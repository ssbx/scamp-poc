/*
 * Efficient catalogs cross matching functions.
 *
 * Copyright (C) 2017 University of Bordeaux. All right reserved.
 * Written by Emmanuel Bertin
 * Written by Sebastien Serre
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */



#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "crossmatch.h"
#include "logger.h"
#include "mem.h"
#include "chealpix.h"

static void insert_object_in_cell(Object*, HealpixCell**, long, long);

/*
 * TODO voir query_disc (fortran). If used, neighbours_nest is useless.
 * TODO voir angdist
 */
void
Crossmatch_crossCells(HealpixCell **cells, long *cellindex,
                        long ncells, double radius) {
    long i;
    int j;
    HealpixCell *current;
    long *neighbors;

    for (i=0; i<ncells; i++) {
        int nneigh = 0;
        current = cells[cellindex[i]];
        neighbors = current->neighbors;

        HealpixCell *neighbor_cell;
        for (j=0; j<8; j++) {
            if (neighbors[j] < 0 || cells[neighbors[j]] == NULL)
                continue;

            neighbor_cell = cells[neighbors[j]];
            nneigh += neighbor_cell->nobjects;
        }

        Logger_log(LOGGER_TRACE,
                "Must match all %li objects with neighbors objects"
                "(%li) for a total of %li matches.\n",
                current->nobjects, nneigh,
                (current->nobjects + nneigh - 1) * current->nobjects);

    }

}


HealpixCell **
Crossmatch_initCells(long nsides) {
    HealpixCell **cells;
    long npix, i;
    npix = nside2npix(nsides);

    Logger_log(LOGGER_DEBUG,
            "Will allocate room for cells. It will take %i MB\n",
            sizeof(HealpixCell*) * npix / 1000000);

    cells = (HealpixCell**) ALLOC(sizeof(HealpixCell*) * npix);

#pragma omp simd
    for (i=0; i<npix; i++) {
        cells[i] = NULL;
    }

    return cells;
}


static int cmp_objects_on_ra(const void *a, const void *b) {
    Object **aa, **bb;
    aa = (Object**) a;
    bb = (Object**) b;
    return (*aa)->ra == (*bb)->ra ? 0 : ((*aa)->ra < (*bb)->ra ? -1 : 1);
}

#define PIX_INDEX_BASE_SISE 1000
long*
Crossmatch_fillCells(Field *fields, int nfields, HealpixCell **cells,
                    long nsides, long *ncells) {
    long i;
    int j, k;
    Field *field;
    Set *set;
    Object *obj;
    HealpixCell *cell;
    long *pixindex;
    long pixindex_size;

    long total_nobjects;
    total_nobjects = 0;
    for (i=0; i<nfields; i++) {
        field = &fields[i];
        for (j=0; j<field->nsets; j++) {
            set = &field->sets[j];
            total_nobjects += set->nobjects;
            for (k=0; k<set->nobjects; k++) {

                obj  = &set->objects[k];
                insert_object_in_cell(obj, cells, obj->pix_nest, nsides);

            }
        }
    }

    Logger_log(LOGGER_DEBUG,
            "Total size for cells is %li MB\n",
            (nside2npix(nsides) * sizeof(HealpixCell*) +
                    total_nobjects * sizeof(Object)) / 1000000);

    pixindex = ALLOC(sizeof(long) * PIX_INDEX_BASE_SISE);
    pixindex_size = PIX_INDEX_BASE_SISE;
    *ncells = 0;
    for (i=0; i<nside2npix(nsides);i++) {
        cell = cells[i];
        if (cell == NULL)
            continue;

        /*
         * Realloc z
         */
        cell->objects = REALLOC(cell->objects,
                                sizeof(Object*) * cell->nobjects);

        /*
         * Sort
         */
        qsort(cell->objects,cell->nobjects,sizeof(Object*),cmp_objects_on_ra);

        /*
         * Append to pixindex
         */
        if (*ncells == pixindex_size) {
            pixindex = REALLOC(pixindex, sizeof(long) * pixindex_size * 2);
            pixindex_size *= 2;
        }
        pixindex[*ncells] = i;
        (*ncells)++;

    }

    pixindex = REALLOC(pixindex, sizeof(long) * pixindex_size);
    return pixindex;
}

void
Crossmatch_freeCells(HealpixCell **cells, long nsides) {
    long i;
    HealpixCell *cell;
    for (i=0; i<nside2npix(nsides);i++) {
        cell = cells[i];
        if (cell != NULL) {
            FREE(cell->objects);
        }
    }
    FREE(cells);
}

/*
 * Static utilities
 */
#define CELL_BASE_SIZE 100
static void
insert_object_in_cell(Object *obj, HealpixCell **cells,
                        long index, long nsides) {
    HealpixCell *cell;

    if (cells[index] == NULL) {
        cells[index] = ALLOC(sizeof(HealpixCell));
        cells[index]->objects = NULL;
        neighbours_nest(nsides, index, cells[index]->neighbors);
    }
    cell = cells[index];

    /* First allocation */

    if (cell->objects == NULL) {
        cell->objects = ALLOC(sizeof(Object*) * CELL_BASE_SIZE);
        cell->size = CELL_BASE_SIZE;
        cell->nobjects = 0;

    } else if (cell->size == cell->nobjects) { /* need realloc */
        cell->objects = REALLOC(cell->objects,
                                sizeof(Object*) * cell->size * 2);
        cell->size *= 2;

    }

    /* append object */
    cell->objects[cell->nobjects] = obj;
    cell->nobjects++;

}

//double
//Object_areClose(Object_T a, Object_T b, double factor) {
//    double distance, limit;
//
//    /* See https://fr.wikipedia.org/wiki/Formule_de_haversine */
//    distance = 2 * asin(sqrt(
//        pow(sin( (b.dec - a.dec) / 2) , 2) +
//        cos(a.dec) * cos(b.dec) *
//        pow(sin((b.ra - a.ra) / 2) , 2)
//    ));
//
//    limit = factor * sqrt(a.sd * a.sd + b.sd * b.sd);
//
//    if (distance < limit)
//        return true;
//    else
//        return false;
//
//}


//void Crossmatch_crossfields(Field *fields, int nfields, ObjectZone **zones) {
//
//    int i, j, k;
//    Object obj;
//    ObjectZone *objzone;
//
//    /* TODO TODO TODO TODO
//     * Iterate over healpix zones, better than objects */
//
//    /*
//     * We are matching all objects from any fields including the
//     * one we are matching on.
//     * TODO filter matches to remove matches from the original
//     * field, and matches coming from the same field for an object
//     * (keep the best).
//     * TODO we could match healpix rings instead.
//     */
//    for (i=0; i<nfields; i++) {
//    	for (j=0; j<fields[i].nsets; j++) {
//    		for (k=0; k<fields[i].sets[j].nobjects; k++) {
//    			obj = fields[i].sets[j].objects[k];
//
//    			/*
//    			 * TODO XXX BUG it is not enough to have matches on border
//    			 * of zones. We must also take adjacent zones.
//    			 */
//    			objzone = zones[obj.pix_nest]; /* zones objects are sorted by right ascension */
//
//    			Logger_log(LOGGER_TRACE,
//    					"Have found %i matches for %i\n",objzone->nobjects,obj.id);
//    		}
//    	}
//    }
//}

//static unsigned long long
//findPositionLT(ObjectCat_T *l, double raMax) {
//	/* TODO optimize with a quick search algorithm */
//	unsigned long long r;
//
//	for (r = l->length - 1; r >= 0; r--) {
//		if (l->objects[r].ra < raMax)
//			break;
//	}
//	return r;
//}
//
//static unsigned long long
//findPositionGT(ObjectCat_T *l, double raMin) {
//	/* TODO optimize with a quick search algorithm */
//	unsigned long long r;
//
//	for (r=0; r<l->length; r++) {
//		if (l->objects[r].ra > raMin)
//			break;
//	}
//	return r;
//}
//
//static void
//filterObjectsByRa(
//		unsigned long long *indexStart,
//		unsigned long long *indexEnd,
//		ObjectCat_T *ref,
//		double ra,
//		double sdMax) {
//
//	/* ObjectList is sorted by ra find the indexes in which ra fits */
//	*indexStart = findPositionGT(ref, ra - sdMax);
//	*indexEnd   = findPositionLT(ref, ra + sdMax);
//
//}
//
///*
// * Take two ObjectCat_T and count the number of match between them.
// */
//void
//Crossmatch_run(
//        ObjectCat_T *cat_A,
//        ObjectCat_T *cat_B,
//		double     factor) {
//
//	Object refObject;
//	Object testObject;
//
//	double sdMaxA, sdMaxB, sdMax;
//
//	int count, matches;
//	unsigned long long i, j, jStart, jEnd;
//
//	sdMaxA = cat_A->maxSd;
//	sdMaxB = cat_B->maxSd;
//	/* WARNING what if dec is max or min? */
//	sdMax = factor * sqrt(sdMaxA * sdMaxA + sdMaxB * sdMaxB);
//
//	count = matches = 0;
//
//	for (i=0; i < cat_B->length; i++) {
//
//		testObject = cat_B->objects[i];
//
//		/* Filter obvious non matches on right ascension */
//		filterObjectsByRa(&jStart, &jEnd, cat_A, testObject.ra, sdMax);
//		if (jEnd > jStart) /* No matching objects */
//			continue;
//
//		for (j = jStart; j <= jEnd; j++) {
//			refObject = reference->objects[j];
//
//			/*
//			 * Eliminate obvious non matches on declination.
//			 * TODO Maybe use a sorted by declination list and cross match merge
//			 * with ra candidates list (maybe sort by pointers both ra and dec
//			 * in the Objectlist_commit function).
//			 */
//			if (
//					(testObject.dec + sdMax) < refObject.dec ||
//					(testObject.dec - sdMax) > refObject.dec
//			) continue;
//
//			/*
//			 * Here, refObject is a very good candidate for matching. It is
//			 * contained in a scare of (sdMax*sdMax) size centered on
//			 * testObject.
//			 */
//			printf("increase count %i\n", omp_get_thread_num());
//			count++;
//			if (Object_areClose(refObject, testObject, factor)) {
//				/* We got a MATCH!!! */
//
//				printf("increase matches %i\n", omp_get_thread_num());
//				matches++;
//			}
//		}
//	}
//
//	printf("Number of matches: %i\n",matches);
//	printf("Number of iterations: %i\n",count);
//
//	return;
//}
//
//

