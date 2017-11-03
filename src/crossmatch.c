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

#include <stdio.h>
#include <math.h>
#include "crossmatch.h"
#include "datum.h"

#define MATCH_DISTANCE_DEGREE 1.0f

/*
 * This is incorrect, wee use pitagore on a sphere.
 * This is not optimized.
 */
void crossmatch_run_naive1(DatumList *reference, DatumList *samples) {
    Datum refDatum;
    Datum splDatum;
    double a, b, c; // pythagore
    int i, j;

    printf("hello crossmatch\n");
    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {
            splDatum = samples->datums[i];
            a = pow(refDatum.ra - splDatum.ra, 2.0f);
            b = pow(refDatum.dec - splDatum.dec, 2.0f);
            c = sqrt(a + b);
            if (c < MATCH_DISTANCE_DEGREE)
                printf("got a hit!\n");
            
        }
    }
    return;
}

/*
 * This is correct, wee use pitagore on cartesian coordinates.
 * This is not optimized.
 */
#define HEARTH_RADIUS_CONST 6000
void crossmatch_run(DatumList *reference, DatumList *samples) {
    Datum refDatum;
    Datum splDatum;
    double a, b, c; // pythagore
    int i, j;

    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {
            splDatum = samples->datums[i];
            a = pow(HEARTH_RADIUS_CONST * cos(refDatum.ra - splDatum.ra), 2.0f);
            b = pow(HEARTH_RADIUS_CONST * sin(refDatum.dec - splDatum.dec), 2.0f);
            c = sqrt(a + b);
            if (c < HEARTH_RADIUS_CONST * cos(MATCH_DISTANCE_DEGREE))
                printf("got a hit!\n");

    
        }
    }
}
