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

#include "crossmatch.h"
#include "logger.h"
#include "mem.h"
#include "chealpix.h"
#include "pixelstore.h"

static void crossmatch(Sample*,Sample*,double);
static long cross_pixels(PixelStore*,double);

static long ntestmatches;

#define NNEIGHBORS 8

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
    nmatches = cross_pixels(pixstore, radius_arcsec);
    PixelStore_free(pixstore);

    return nmatches;

}

extern double
Crossmatch_getAveragePixelSize(int64_t nsides) {

    int i;
    double v1[3], v2[3];
    int64_t neighbors[8], neighbor = -1;
    neighbours_nest64(nsides, 0, neighbors);
    for (i=0; i<8; i++) {
        if (neighbors[i] > -1)
            neighbor = neighbors[i];
        break;
    }
    pix2vec_ring64(nsides, 0, v1);
    pix2vec_ring64(nsides, neighbor, v2);
    return angdist(v1, v2) * 180 / SC_PI * 3600 * 60;

}

static long
cross_pixels(PixelStore *store, double radius_arcsec) {
    long i;
    long nbmatches = 0;

    long npixels = store->npixels;
    int64_t *pixelindex = store->pixelids;

    /* arcsec to radiant */
    double radius = radius_arcsec / 3600 * TO_RAD;

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
        long nmatches = 0;
        long j, k, l;

        Sample *current_spl;
        Sample *test_spl;


        for (j=0; j<current_pix->nsamples; j++) {
            current_spl = current_pix->samples[j];
            current_spl->bestMatchDistance = radius;

            /*
             * First cross match with samples of the pixel between them
             */
            for(k=0; k<current_pix->nsamples; k++) {
                test_spl = current_pix->samples[k];

                crossmatch(current_spl, test_spl, radius);

            }

            /*
             * Then iterate against neighbors pixels
             */
            HealPixel *test_pixel;
            for (k=0; k<NNEIGHBORS; k++) {
                test_pixel = current_pix->pneighbors[k];

                /*
                 * Does the pixel exists? It may be a neighbor of current pixel,
                 * but not be initialized because it does not contains
                 * any samples.
                 */
                if (test_pixel == NULL)
                    continue;

                /*
                 * Then iterate over samples.
                 */
                for (l=0; l<test_pixel->nsamples; l++) {
                    test_spl = test_pixel->samples[l];
                    crossmatch(current_spl, test_spl, radius);

                }
            }

            if (current_spl->bestMatch != NULL) {
                nmatches++;
                nbmatches++;
            }
        }

        Logger_log(LOGGER_DEBUG,
                "Crossmatch end: %li matches for pixel %li!\n", nmatches,i);
    }
    Logger_log(LOGGER_NORMAL,
            "Crossmatch end: %li matches for all pixels!\n", nbmatches);
    Logger_log(LOGGER_NORMAL,
            "Crossmatch end: %li real cross match tests!\n", ntestmatches);
    return nbmatches;
}


static void
crossmatch(Sample *current_spl, Sample *test_spl, double radius) {

    ntestmatches++;
    /*
     * pass if sample is of the same field
     */
    if (current_spl->set->field == test_spl->set->field)
        return;

    /*
     * pass if dec is not in a good range
     */
    if (abs(current_spl->col - test_spl->col) > radius)
        return;

    /*
     * Cross match then!
     *
     * Get distance between samples (rad)
     */
    double distance_rad = angdist(current_spl->vector, test_spl->vector);

    /*
     * If distance is less than previous (or radius) update
     * best_distance and set test_spl to current_spl.bestMatch
     */
    if (distance_rad < current_spl->bestMatchDistance) {
        current_spl->bestMatch = test_spl;              /* XXX false shared ! */
        current_spl->bestMatchDistance = distance_rad;  /* XXX false shared ! */
    }

}


