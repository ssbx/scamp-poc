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
#include <omp.h>
#include <stdio.h>

#include "crossmatch.h"
#include "logger.h"

void Crossmatch_cross(Field *black, Field *white) {

    int i, j;

    /*
     * We are matching obj_black agains all objects of the same zone
     * (zone_white) from the field white.
     */
    Object      *obj_black;
    ObjectZone  *zone_white;

    for (i=0; i<black->nsets; i++) {
        for (j=0; j<black->sets[i].nobjects; j++) {
            obj_black = &black->sets[i].objects[j];
            zone_white = &white->ipring_zone[obj_black->ipring];
//            if (zone_white->nobjects != 0)
//                Logger_log(LOGGER_DEBUG,
//                        "Black Object %li will try matching %i objects from field white\n",
//                        obj_black->id, zone_white->nobjects);

        }
    }
}

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
// pragma omp parallel for \
//		reduction(+:matches), \
//		reduction(+:count), \
//		shared(samples, reference), \
//		private(i, j, testObject, refObject, jEnd, jStart)
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
//bool
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
