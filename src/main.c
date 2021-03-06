/*
 * Copyright (C) 2017 University of Bordeaux. All right reserved.
 * Written by Emmanuel Bertin
 * Written by Sebastien Serre
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "mem.h"
#include "logger.h"
#include "catalog.h"
#include "crossmatch.h"
#include "pixelstore.h"

#include "chealpix.h"
#include "scamp.h"

/**
 * TODO:
 * 1 - get nsides depending of the max error from all files with the neighbors
 * algo. (see dist2holes_nest fortran)
 * 2 - cross/match with as little as possible samples (see query_ring?).
 */
int main(int argc, char** argv) {

    Logger_setLevel(LOGGER_NORMAL);

    /* default values */
    int nsides_power = 16, c;
    double radius_arcsec = 2.0; /* in arcsec */
	int nthreads= 4;

    while ((c=getopt(argc,argv,"n:r:t:b")) != -1) {
        switch(c) {
        case 'n':
            nsides_power = atoi(optarg);
            break;
        case 'r':
            radius_arcsec = atof(optarg);
            break;
        case 't':
            nthreads = atoi(optarg);
            break;
        default:
            abort();
        }
    }

    int nfields   = argc - optind;
    char **cat_files = &argv[optind];

    Field *fields = ALLOC(sizeof(Field) * nfields);

    int64_t nsides = pow(2, nsides_power);
    PixelStore *store = PixelStore_new(nsides);
    int i;
    for (i=0; i<nfields; i++)
        Catalog_open(cat_files[i], &fields[i], store);

    struct timespec start, end;
	printf("match radius max is %0.30lf\n", (180.0f / (4 * nsides - 1)) * 3600  );
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    Crossmatch_crossSamples(store, radius_arcsec, nthreads);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    int sec = end.tv_sec - start.tv_sec;
    double nano = (end.tv_nsec - start.tv_nsec);
    double nano2 = nano / 1000000000;
    double elapsed = (double) sec + nano2;
    printf("Crossmatch done in %lf time seconds\n", elapsed);

    for (i=0; i<nfields; i++)
        Catalog_freeField(&fields[i]);

    PixelStore_free(store);
    return (EXIT_SUCCESS);

}

