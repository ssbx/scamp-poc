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

#include "crossmatch.h"
#include "logger.h"
#include "mem.h"
#include "chealpix.h"
#include "pixelstore.h"

static void crossmatch(Sample*,Sample*);
static long cross_pixels(PixelStore*,double);

static long ntestmatches;


#define NNEIGHBORS 8

/**
 * A match bundle contains every samples from any fields that match each others.
 * This include friend-of-friends objects.
 */
typedef struct MatchTest {
    Sample *a, *b;
    double va[3], vb[3];
    double distance;
} MatchTest;

extern long
Crossmatch_crossFields(
        Field           *fields,
        int             nfields,
        int64_t         nsides,
        double          radius_arcsec)
{
    long nmatches;

    PixelStore *pixstore;
    pixstore = PixelStore_new(fields, nfields, nsides);
    /* arcsec to radiant */
    double radius = radius_arcsec / 3600 * TO_RAD;

    PixelStore_setMaxRadius(pixstore, radius);
    clock_t start, end;
    start = clock();
    nmatches = cross_pixels(pixstore, radius);
    end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Crossmatch (only) done in %lf cpu_time seconds\n", cpu_time_used);
    PixelStore_free(pixstore);


    return nmatches;

}

/**
 * Test if two pixels have already tested cross matching samples. If not,
 * set it to true.
 */
bool
test_allready_crossed(HealPixel *a, HealPixel *b, int an) {
    if (a->tneighbors[an] == true) // already matched
        return true;

    a->tneighbors[an] = true;
    int i;
    for (i=0; i<8; i++) {
        if (b->pneighbors[i] == a)
            b->tneighbors[i] = true;
        break;
    }
    return false;
}

#define BASE_MATCH_TEST_SIZE 1000
static long
cross_pixels(PixelStore *store, double radius) {
    long i;
    long nbmatches = 0;

    long npixels = store->npixels;
    int64_t *pixelindex = store->pixelids;

    /*
     * Iterate over HealPixel structure which old pointers to sample
     * belonging to him.
     *
     * Define functions inside loop, for future omp usage.
     *
     * XXX TODO: pixels are crossed twice (one as current_pix, and another
     * as neighbor_pix) with each others,
     * XXX TODO: how should I link matching samples from different fields? (
     * answer, use the MatchBundle structure. Only one sample from the same
     * field must exist in one MatchBundle. Maybe add all, and reduce after,
     * or find a way to reduce. Do not modify Sample structure while iterating,
     * but reduce at the end to avoid false sharing.
     * XXX TODO: this code is thread safe but not parallelisable, because of the
     * false sharing induced by the "crossmatch" function modifying some
     * Sample values. TODO, create a temporary result for each threads, then
     * reduce the results in a single thread.
     */
    for (i=0; i<npixels; i++) {

        HealPixel *current_pix = PixelStore_get(store,pixelindex[i]);

        long j, k, l;

        Sample *current_spl;
        Sample *test_spl;

        for (j=1; j<current_pix->nsamples; j++) {

            current_spl = current_pix->samples[j];

            /*
             * First cross match with samples of the pixel between them
             */
            for(k=0; k<j; k++) {
                test_spl = current_pix->samples[k];

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
                test_pixel = current_pix->pneighbors[k];

                /*
                 * Does the pixel contains any samples?
                 */
                if (test_pixel == NULL)
                    continue;

                if (test_allready_crossed(current_pix, test_pixel, k) == true)
                    continue;

                /*
                 * Then iterate over samples.
                 */
                for (l=0; l<test_pixel->nsamples; l++) {
                    test_spl = test_pixel->samples[l];

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
    }

    Logger_log(LOGGER_NORMAL,
            "Crossmatch end: %li matches for all pixels!\n", nbmatches);
    Logger_log(LOGGER_NORMAL,
            "Crossmatch end: %li real cross match tests!\n", ntestmatches);
    return nbmatches;

}

int
get_iterate_count() {
    return ntestmatches;
}

static void
crossmatch(Sample *current_spl, Sample *test_spl) {

    ntestmatches++;

    /*
     * Get distance between samples
     */
    double distance = euclidean_distance(current_spl->vector, test_spl->vector);

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


