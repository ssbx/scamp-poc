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
#include <omp.h>

#include "global.h"
#include "crossmatch.h"
#include "logger.h"
#include "mem.h"
#include "chealpix.h"
static void insert_object_in_cell(Object*, HealpixCell**, long, long);
static void crossmatch(Object* current, Object* test, double radius);
static void crossmatch_querydisc_algo(HealpixCell **cells, long *cellindex,
                        long ncells, double radius_arcsec);
static void crossmatch_neighbors_algo(HealpixCell **cells, long *cellindex,
                        long ncells, double radius_arcsec);
#define NNEIGHBORS 8



void
Crossmatch_crossCells(HealpixCell **cells, long *cellindex,
                        long ncells, double radius_arcsec, CrossmatchAlgo algo) {
    switch (algo) {
    case ALGO_NEIGHBORS:
        crossmatch_neighbors_algo(cells,cellindex,ncells,radius_arcsec);
        break;
    case ALGO_QUERYDISC:
        crossmatch_querydisc_algo(cells,cellindex,ncells,radius_arcsec);
        break;
    }
}

static void
crossmatch_querydisc_algo(HealpixCell **cells, long *cellindex,
                        long ncells, double radius_arcsec) {
    /*
     * TODO see query_disc (fortran) as an alternative method.
     * - 1 implement it (and tests)
     * - 2 try!
     */

}

static void
crossmatch_neighbors_algo(HealpixCell **cells, long *cellindex,
                        long ncells, double radius_arcsec) {
    long i;

    long nbmatches = 0;
    /*
     * Iterate over HealpixCell structure which old pointers to objects
     * belonging to him.
     *
     * Define functions inside loop, for future omp usage.
     */
    for (i=0; i<ncells; i++) {
        /* arcsec to radiant */
        double radius = radius_arcsec / 3600 * SC_PI_DIV_180;

        HealpixCell *current_cell = cells[cellindex[i]];
        long nmatches = 0;
        long j, k, l;
        long *neighbor_cells = current_cell->neighbors;

        Object *current_obj;
        Object *test_obj;


        for (j=0; j<current_cell->nobjects; j++) {
            current_obj = current_cell->objects[j];
            current_obj->bestMatchDistance = radius;

            /*
             * First cross match with objects of the cell between them
             */
            for(k=0; k<current_cell->nobjects; k++) {
                test_obj = current_cell->objects[k];

                crossmatch(current_obj, test_obj, radius);

            }

            /*
             * Then iterate against neighbors cells
             */
            long neighbor_indexes;
            HealpixCell *test_cell;
            for (k=0; k<NNEIGHBORS; k++) {
                neighbor_indexes = neighbor_cells[k];

                /*
                 * Continue if there is no such neighbor pixel can have
                 * 7 on 8 neighbors. Non existing cells are set to -1
                 */
                if (neighbor_indexes < 0)
                    continue;

                test_cell = cells[neighbor_indexes];
                /*
                 * Does the cell exists? It may be a neighbor of current cell,
                 * but not be initialized because it does not contains
                 * any objects.
                 */
                if (test_cell == NULL)
                    continue;

                /*
                 * Then iterate over objects.
                 */
                for (l=0; l<test_cell->nobjects; l++) {
                    test_obj = test_cell->objects[l];
                    crossmatch(current_obj, test_obj, radius);

                }
            }

            if (current_obj->bestMatch != NULL) {
                nmatches++;
                nbmatches++;
            }
        }

        /*
         * For debugging purpose only
         */
//        HealpixCell *neighbor_cell = cells[cellindex[i]];
//
//        for (j=0; j<8; j++) {
//            if (neighbors[j] < 0 || cells[neighbors[j]] == NULL)
//                continue;
//            neighbor_cell = cells[neighbors[j]];
//            nneigh += neighbor_cell->nobjects;
//        }
//
//        Logger_log(LOGGER_TRACE,
//                "Have matched all %li objects with neighbors objects"
//                "(%li) for a total of %li matches.\n",
//                current->nobjects, nneigh,
//                (current->nobjects + nneigh - 1) * current->nobjects);

        Logger_log(LOGGER_DEBUG,
                "Crossmatch end. Got %li matches for cell %li!\n", nmatches,i);
    }
    Logger_log(LOGGER_NORMAL,
            "Crossmatch end. Got %li matches for all cells!\n", nbmatches);

}

static void
crossmatch(Object *current_obj, Object *test_obj, double radius) {
    /*
     * pass if object is of the same field
     */
    if (current_obj->set->field == test_obj->set->field)
        return;

    /*
     * pass if dec is not in a good range
     */
//    if (abs(current_obj->dec - test_obj->dec) > radius)
//        return;

    /*
     * Cross match then!
     *
     * Get distance between objects (rad)
     */
    double distance_rad = angdist(current_obj->vector, test_obj->vector);

    /*
     * If distance is less than previous (or radius) update
     * best_distance and set test_obj to currentObj.bestMatch
     */
    if (distance_rad < current_obj->bestMatchDistance) {
        current_obj->bestMatch = test_obj;
        current_obj->bestMatchDistance = distance_rad;
    }

}


HealpixCell **
Crossmatch_initCells(long nsides) {
    HealpixCell **cells = NULL;
    long npix = nside2npix(nsides);

    Logger_log(LOGGER_NORMAL,
            "Will allocate room for %li cells. It will take %i MB\n",
            npix, sizeof(HealpixCell*) * npix / 1000000);

    cells = (HealpixCell**) CALLOC(npix, sizeof(HealpixCell*));

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
                obj->bestMatch = NULL;
                ang2pix_nest(nsides, obj->dec, obj->ra, &obj->pix_nest);
                ang2vec(obj->dec, obj->ra, obj->vector);
                insert_object_in_cell(obj, cells, obj->pix_nest, nsides);

            }
        }
    }

    Logger_log(LOGGER_TRACE,
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
Crossmatch_freeCells(HealpixCell **cells, long *cellindex, long ncells) {
    long i;
    for (i=0; i<ncells; i++) {
        FREE(cells[cellindex[i]]->objects);
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

