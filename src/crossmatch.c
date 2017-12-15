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

typedef struct SortedField {
    Sample **x;
    Sample **y;
    Sample **z;
} SortedField;

static int cmpx(const void *p1, const void *p2) {
    return (((Sample*)p1)->vector[0] < ((Sample*)p2)->vector[0]) ? -1 : 1;}
static int cmpy(const void *p1, const void *p2) {
    return (((Sample*)p1)->vector[1] < ((Sample*)p2)->vector[1]) ? -1 : 1;}
static int cmpz(const void *p1, const void *p2) {
    return (((Sample*)p1)->vector[2] < ((Sample*)p2)->vector[2]) ? -1 : 1;}

#define NNEIGHBORS 8
extern long
Crossmatch_crossFields2(
        Field           *fields,
        int             nfields,
        int64_t         nsides,
        double          radius_arcsec)
{
    int i, j, k;
    SortedField f[nfields];

    for (i=0; i<nfields; i++) {
        long nsamples = 0;
        for (j=0; j<fields->nsets; j++)
            nsamples += fields->sets[j].nsamples;
        f[i].x = ALLOC(sizeof(Sample*) * nsamples);
        f[i].y = ALLOC(sizeof(Sample*) * nsamples);
        f[i].z = ALLOC(sizeof(Sample*) * nsamples);
        long spos = 0;
        for (j=0; j<fields[i].nsets; j++) {
            for (k=0; k<fields[i].sets[j].nsamples; k++) {
                Sample *spl = &fields[i].sets[j].samples[k];
                ang2vec(spl->col, spl->lon, spl->vector);
                f[i].x[spos] = spl;
                f[i].y[spos] = spl;
                f[i].z[spos] = spl;
                spos++;
            }
        }
        qsort(f[i].x, sizeof(Sample*), nsamples, cmpx);
        qsort(f[i].y, sizeof(Sample*), nsamples, cmpy);
        qsort(f[i].z, sizeof(Sample*), nsamples, cmpz);
    }

    for (i=0; i<nfields; i++) {
        FREE(f[i].x);
        FREE(f[i].y);
        FREE(f[i].z);
    }
    return 0;
}

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


