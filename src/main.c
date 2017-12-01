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

#include "global.h"
#include "logger.h"
#include "catalog.h"
#include "crossmatch.h"

#include "chealpix.h"

/**
 * TODO:
 * 1 - get nsides depending of the max error from all files. (see
 * dist2holes_nest fortran)
 * 2 - crossmatch with as little as possible objects (see in_ring?),
 *
 */
int main(int argc, char** argv) {
    int i, j, nfields;
    clock_t c;
    long nsides = 8192; /* a power of 2 */
    long *cellindex, ncells;
    double radius_arcsec = 2.0; /* in arcsec */

    if (argc < 3)
        Logger_log(LOGGER_CRITICAL, "Require two file arguments\n");

    Logger_setLevel(LOGGER_DEBUG);

    nfields = 10;
    Field fields[10];
    c = clock();
    Catalog_open(argv[1], &fields[0]);
    Catalog_open(argv[1], &fields[1]);
    Catalog_open(argv[1], &fields[2]);
    Catalog_open(argv[1], &fields[3]);
    Catalog_open(argv[1], &fields[4]);
    Catalog_open(argv[2], &fields[5]);
    Catalog_open(argv[2], &fields[6]);
    Catalog_open(argv[2], &fields[7]);
    Catalog_open(argv[2], &fields[8]);
    Catalog_open(argv[2], &fields[9]);
//
//    Catalog_open(argv[1], &fields[10]);
//    Catalog_open(argv[1], &fields[11]);
//    Catalog_open(argv[1], &fields[12]);
//    Catalog_open(argv[1], &fields[13]);
//    Catalog_open(argv[1], &fields[14]);
//    Catalog_open(argv[2], &fields[15]);
//    Catalog_open(argv[2], &fields[16]);
//    Catalog_open(argv[2], &fields[17]);
//    Catalog_open(argv[2], &fields[18]);
//    Catalog_open(argv[2], &fields[19]);
//
//    Catalog_open(argv[1], &fields[20]);
//    Catalog_open(argv[1], &fields[21]);
//    Catalog_open(argv[1], &fields[22]);
//    Catalog_open(argv[1], &fields[23]);
//    Catalog_open(argv[1], &fields[24]);
//    Catalog_open(argv[2], &fields[25]);
//    Catalog_open(argv[2], &fields[26]);
//    Catalog_open(argv[2], &fields[27]);
//    Catalog_open(argv[2], &fields[28]);
//    Catalog_open(argv[2], &fields[29]);
    /* load fields */
//    for (i = 0, j = 1; i < nfields; i++, j++)
//        Catalog_open(argv[j], &fields[i], nsides);
    c = clock() - c;
    printf("Took %f seconds for opening catalogs\n", (double)c / CLOCKS_PER_SEC);

    /* create an effective hash like cell array ... */
    c = clock();
    HealpixCell **cells = Crossmatch_initCells(nsides);
    c = clock() - c;
    printf("Took %f seconds to allocate cell array\n", (double)c / CLOCKS_PER_SEC);

    c = clock();
    cellindex = Crossmatch_fillCells(fields, nfields, cells, nsides, &ncells);
    c = clock() - c;
    printf("Took %f seconds to fill cells with fields values\n", (double)c / CLOCKS_PER_SEC);

    /* ... that will speed up cross matching */
    c = clock();
    Crossmatch_crossCells(cells, cellindex, ncells, radius_arcsec, ALGO_NEIGHBORS);
    c = clock() - c;
    printf("Took %f seconds to cross match cell objects\n", (double)c / CLOCKS_PER_SEC);

    /* cleanup */
    c = clock();
    for (i = 0; i < nfields; i++)
        Catalog_freeField(&fields[i]);
    Crossmatch_freeCells(cells, nsides);
    c = clock() - c;
    printf("Took %f seconds cleaning up\n", (double)c / CLOCKS_PER_SEC);


    return (EXIT_SUCCESS);

}

