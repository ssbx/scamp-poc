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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <omp.h>
#include "crossmatch.h"
#include "catalog/object.h"


static unsigned long long
findPositionLT(ObjectList_T *l, double raMax) {
	/* TODO optimize with a quick search algorithm */
	unsigned long long r;

	for (r=l->size - 1; r >= 0; r--) {
		if (l->objects[r].ra < raMax)
			break;
	}
	return r;
}

static unsigned long long
findPositionGT(ObjectList_T *l, double raMin) {
	/* TODO optimize with a quick search algorithm */
	unsigned long long r;

	for (r=0; r<l->size; r++) {
		if (l->objects[r].ra > raMin)
			break;
	}
	return r;
}

static void
filterObjectsByRa(
		unsigned long long *indexStart,
		unsigned long long *indexEnd,
		ObjectList_T *ref,
		double ra,
		double sdMax) {

	/* ObjectList is sorted by ra find the indexes in which ra fits */
	*indexStart = findPositionGT(ref, ra - sdMax);
	*indexEnd   = findPositionLT(ref, ra + sdMax);

}

/*
 * Take two ObjectList and count the number of match between them.
 */
void
Crossmatch_run(
		ObjectList_T *reference,
		ObjectList_T *samples,
		double        factor) {

	Object_T refObject;
	Object_T testObject;

	double sdMaxRef, sdMaxTest, sdMax;

	int count, matches;
	unsigned long long i, j, jStart, jEnd;

	sdMaxRef  = Objectlist_getMaxSd(reference);
	sdMaxTest = Objectlist_getMaxSd(samples);
	/* WARNING what if dec is max or min? */
	sdMax     = factor * sqrt(sdMaxRef * sdMaxRef + sdMaxTest * sdMaxTest);

	count = matches = 0;

	/* TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	 * Divide job for threads. Two possibilities:
	 * - divide in N parts (using sdMax and Healpix) and use threads. Then
	 * merge the results,
	 * - directly divide jobs with for and openmp. Reference catalogs should be
	 * big for this to be viable. Load of filling high number of jobs to
	 * openmp must be taken in consideration.
	 */
#pragma omp parallel for \
		reduction(+:matches), \
		reduction(+:count), \
		shared(samples, reference), \
		private(i, j, testObject, refObject, jEnd, jStart)
	for (i=0; i < samples->size; i++) {
		testObject = samples->objects[i];

		/* Filter obvious non matches on right ascension */
		filterObjectsByRa(&jStart, &jEnd, reference, testObject.ra, sdMax);
		if (jEnd > jStart) /* No matching objects */
			continue;

		for (j = jStart; j <= jEnd; j++) {
			refObject = reference->objects[j];

			/*
			 * Eliminate obvious non matches on declination.
			 * TODO Maybe use a sorted by declination list and cross match merge
			 * with ra candidates list (maybe sort by pointers both ra and dec
			 * in the Objectlist_commit function).
			 */
			if (
					(testObject.dec + sdMax) < refObject.dec ||
					(testObject.dec - sdMax) > refObject.dec
			) continue;

			/*
			 * Here, refObject is a very good candidate for matching. It is
			 * contained in a scare of (sdMax*sdMax) size centered on
			 * testObject.
			 */

			printf("increase count %i\n", omp_get_thread_num());
			count++;
			if (Object_areClose(refObject, testObject, factor)) {
				/* We got a MATCH!!! */

				printf("increase matches %i\n", omp_get_thread_num());
				matches++;
			}

		}
	}

	printf("Number of matches: %i\n",matches);
	printf("Number of iterations: %i\n",count);

	return;
}
