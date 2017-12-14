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
     * Iterate over HealPixel structure which holds pointers to sample
     * belonging to him.
     */
    for (i=0; i<npixels; i++) {

        HealPixel *current_pix = PixelStore_get(store,pixelindex[i]);

        long j, k;

        Sample *current_spl;
        Sample *closer_spl;

        for (j=1; j<current_pix->nsamples; j++) {

            current_spl = current_pix->samples[j];

            closer_spl = PixelStore_getClosestSample(current_pix, current_spl);

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


                /*
                 * We may allready crossed these two pixels
                 */
                if (test_allready_crossed(current_pix, test_pixel, k) == true)
                    continue;

                closer_spl = PixelStore_getClosestSample(test_pixel, current_spl);

            }
        }
    }
    return nbmatches;
}

int
get_iterate_count() {
    return ntestmatches;
}


