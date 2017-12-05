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
#include "pixelstore.h"

static void crossmatch(Object*, Object*, double);
static void crossmatch_querydisc_algo(PixelStore*,double);
static void crossmatch_neighbors_algo(PixelStore*,double);
static void cross_cells(PixelStore*,double,CrossmatchAlgo);

#define NNEIGHBORS 8

extern void
Crossmatch_crossFields(
        Field           *fields,
        int             nfields,
        long            nsides,
        double          radius_arcsec,
        CrossmatchAlgo  algo,
        StoreScheme     scheme)
{

    PixelStore *pixstore;
    pixstore = PixelStore_new(fields, nfields, nsides, scheme);
    cross_cells(pixstore, radius_arcsec, algo);
    PixelStore_free(pixstore);
}


static void
cross_cells(PixelStore *pixstore, double radius_arcsec, CrossmatchAlgo algo) {
    switch (algo) {
    case ALGO_NEIGHBORS:
        crossmatch_neighbors_algo(pixstore,radius_arcsec);
        break;
    case ALGO_QUERYDISC:
        crossmatch_querydisc_algo(pixstore,radius_arcsec);
        break;
    }
}

static void
crossmatch_querydisc_algo(PixelStore *pixstore, double radius_arcsec) {
    /*
     * TODO see query_disc (fortran) as an alternative method.
     * - 1 implement it (and tests)
     * - 2 try!
     */

}

static void
crossmatch_neighbors_algo(PixelStore *store, double radius_arcsec) {
    long i;
    long nbmatches = 0;

    long ncells = store->npixels;
    long *cellindex = store->pixelids;

    /*
     * Iterate over HealpixCell structure which old pointers to objects
     * belonging to him.
     *
     * Define functions inside loop, for future omp usage.
     */
    for (i=0; i<ncells; i++) {
        /* arcsec to radiant */
        double radius = radius_arcsec / 3600 * SC_PI_DIV_180;

        HealPixel *current_cell = PixelStore_get(store,cellindex[i]);
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
            HealPixel *test_cell;
            for (k=0; k<NNEIGHBORS; k++) {
                neighbor_indexes = neighbor_cells[k];

                /*
                 * Continue if there is no such neighbor pixel can have
                 * 7 on 8 neighbors. Non existing cells are set to -1
                 */
                if (neighbor_indexes < 0)
                    continue;

                test_cell = PixelStore_get(store, neighbor_indexes);
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
    if (abs(current_obj->dec - test_obj->dec) > radius)
        return;

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



