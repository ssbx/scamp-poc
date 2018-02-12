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
#include <time.h>
#include <pthread.h>

#include "crossmatch.h"
#include "logger.h"
#include "mem.h"
#include "chealpix.h"
#include "pixelstore.h"

static void crossmatch(Sample*,Sample*);
static long cross_pixel(HealPixel*,PixelStore*,double);

static long ntestmatches;

#define NNEIGHBORS 8

struct thread_args {
	PixelStore 	*store;
	int64_t		*pixelindex;
	int			npixs;
	double 		radius;
	int			*result;
};

void*
pthread_cross_pixel(void *args) {
	struct thread_args *ta = (struct thread_args*) args;

	int i;
	int nmatches = 0;
	for (i=0; i<ta->npixs; i++) {
		HealPixel *pix = PixelStore_get(ta->store, ta->pixelindex[i]);
		nmatches += cross_pixel(pix, ta->store, ta->radius);
	}

	*(ta->result) = nmatches;
	return NULL;
}

long
Crossmatch_crossSamples(
        PixelStore      *pixstore,
        double          radius_arcsec)
{

    /* arcsec to radiant */
    double radius = radius_arcsec / 3600 * TO_RAD;
    PixelStore_setMaxRadius(pixstore, radius);

	int i;
    long nmatches = 0;
	int64_t *pixelindex = pixstore->pixelids;

	int nthreads = 4;
	pthread_t *threads 		 = ALLOC(sizeof(pthread_t) * nthreads);
	struct thread_args *args = ALLOC(sizeof(struct thread_args) * nthreads);
	int	*results			 = ALLOC(sizeof(int) * nthreads);

	int *npixs = ALLOC(sizeof(int) * nthreads);
	int pix_per_thread = pixstore->npixels / nthreads;
	for (i=0; i<nthreads; i++)
		npixs[i] = pix_per_thread;
	npixs[0] += pixstore->npixels % nthreads;

	for (i=0; i<nthreads; i++) {
		struct thread_args *arg = &args[i];
		arg->store 		= pixstore;
		arg->radius 	= radius;
		arg->pixelindex = pixelindex;
		arg->npixs 		= npixs[i];
		arg->result 	= &results[i];
		pthread_create(&threads[i], NULL, pthread_cross_pixel, arg);
		pixelindex += npixs[i];
	}


	for (i=0; i<nthreads; i++)
		pthread_join(threads[i], NULL);

	FREE(threads);
	FREE(args);
	FREE(npixs);
	
	for (i=0; i<nthreads; i++)
		nmatches += results[i];
	FREE(results);

    return nmatches;

}

/**
 * Test if two pixels have already tested cross matching samples. If not,
 * set it to true.
 */
static bool
test_allready_crossed(HealPixel *a, HealPixel *b, int n) {
    if (a->tneighbors[n] == true) // already matched
        return true;

    a->tneighbors[n] = true;
    int i;
    for (i=0; i<NNEIGHBORS; i++) {
        if (b->pneighbors[i] == a) {
            b->tneighbors[i] = true;
        	break;
		}
    }
    return false;
}

static long
cross_pixel(HealPixel *pix, PixelStore *store, double radius) {
	long nbmatches = 0;

	/*
	 * Iterate over HealPixel structure which old sample structures
	 * belonging to him.
	 */
	long j, k, l;

	Sample *current_spl;
	Sample *test_spl;


	for (j=0; j<pix->nsamples; j++) {
		current_spl = &pix->samples[j];

		/*
		 * First cross match with samples of the pixel between them
		 */
		for(k=0; k<j; k++) {
			test_spl = &pix->samples[k];

			if (current_spl->set->field == test_spl->set->field)
				continue;

			if (abs(current_spl->col - test_spl->col) > radius)
				continue;

			crossmatch(current_spl, test_spl);

		}

		/*
		 * Then iterate against neighbors pixels
		 */
		HealPixel *test_pixel;
		for (k=0; k<NNEIGHBORS; k++) {
			test_pixel = pix->pneighbors[k];

			/*
			 * Does the pixel exists? It may be a neighbor of current pixel,
			 * but not be initialized because it does not contains
			 * any samples.
			 */
			if (test_pixel == NULL)
				continue;

			if (test_allready_crossed(pix, test_pixel, k) == true)
				continue;

			/*
			 * Then iterate over samples.
			 */
			for (l=0; l<test_pixel->nsamples; l++) {
				test_spl = &test_pixel->samples[l];

				if (current_spl->set->field == test_spl->set->field)
					continue;

				if (abs(current_spl->col - test_spl->col) > radius)
					continue;

				crossmatch(current_spl, test_spl);

			}
		}

		if (current_spl->bestMatch != NULL) {
			nbmatches++;
		}
	}
	Logger_log(LOGGER_NORMAL,
			"Crossmatch end: %li matches for all pixels!\n", nbmatches);
	return nbmatches;

}

int
get_iterate_count() {
    return ntestmatches;
}

static inline double
dist(double *va, double *vb) {
    double x = va[0] - vb[0];
    double y = va[1] - vb[1];
    double z = va[2] - vb[2];

	/* return x*x + y*y + z*z; */
    return sqrt(x*x + y*y + z*z);
}

static void
crossmatch(Sample *current_spl, Sample *test_spl) {
    ntestmatches++;
    /*
     * Get distance between samples
     */
    double distance = dist(current_spl->vector, test_spl->vector);

    /*
     * If distance is less than previous (or initial) update
     * best_distance and set test_spl to current_spl.bestMatch
     */
    if (distance < current_spl->bestMatchDistance) {
        current_spl->bestMatch = test_spl;          /* XXX false shared ! */
        current_spl->bestMatchDistance = distance;  /* XXX false shared ! */
    }

    if (distance < test_spl->bestMatchDistance) {
        test_spl->bestMatch = current_spl;
        test_spl->bestMatchDistance = distance;
    }

}
