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
#include <stdlib.h>
#include <math.h>
#include "crossmatch.h"
#include "datum.h"

#define MATCH_DISTANCE_DEGREE 0.01f
#define HEARTH_RADIUS_CONST 6000
/*
 * This is incorrect, wee use pitagore on a sphere.
 * This is not optimized.
 */
void crossmatch_bad(DatumList *reference, DatumList *samples) {
    Datum refDatum;
    Datum splDatum;
    double a, b, c; // pythagore
    int i, j;

    int count = 0;
    printf("hello crossmatch\n");
    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {
            splDatum = samples->datums[j];
            a = pow(refDatum.ra - splDatum.ra, 2.0f);
            b = pow(refDatum.dec - splDatum.dec, 2.0f);
            c = sqrt(a + b);
            if (c < MATCH_DISTANCE_DEGREE)
                count++;
            
        }
    }
    printf("count is %i\n",count);
    return;
}

/*
 * It is correct. The distance is expressed in degree.
 * This is not optimized.
 *
 * See https://math.stackexchange.com/questions/833002/distance-between-two-points-in-spherical-coordinates
 */
void crossmatch_all_spherical(DatumList *reference, DatumList *samples) {
    Datum refDatum;
    Datum splDatum;
    int i, j;
    double distance;

    int count = 0;
    printf("hello crossmatch\n");
    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {
            splDatum = samples->datums[j];
            distance = sqrt(
                sin(refDatum.ra) * sin(splDatum.ra) *
                cos(refDatum.dec - splDatum.dec) +
                cos(refDatum.ra) * cos(splDatum.ra)
           ); 
            if (distance < MATCH_DISTANCE_DEGREE) {
                count++;
            }
            
        }
    }
    printf("count is %i\n",count);
    return;
}


void crossmatch_run(DatumList *reference, DatumList *samples) {
    crossmatch_bad(reference, samples);
    crossmatch_all_spherical(reference, samples);
}
