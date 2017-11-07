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
#define PI 3.14
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
    printf("hello bad flat\n");
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
 * It is wrong, sin and coss take radians.
 * This is not optimized.
 *
 * See https://math.stackexchange.com/questions/833002/distance-between-two-points-in-spherical-coordinates
 */
void crossmatch_all_spherical(DatumList *reference, 
                              DatumList *samples, double distance_max) {
    Datum refDatum;
    Datum splDatum;
    int i, j;
    double distance;

    int count = 0;
    printf("hello bad sphere\n");
    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {
            splDatum = samples->datums[j];
            distance = sqrt(
                sin(refDatum.ra) * sin(splDatum.ra) *
                cos(refDatum.dec - splDatum.dec) +
                cos(refDatum.ra) * cos(splDatum.ra)
           ); 
            if (distance < distance_max) {
                count++;
            }
        }
    }
    printf("count is %i\n",count);
    return;
}



static inline double haversine(double ra_A, double dec_A,
                 double ra_B, double dec_B) {
    /* see https://fr.wikipedia.org/wiki/Formule_de_haversine */
    return (2 * (asin(pow(sin((dec_B - dec_A)/2),2) + cos(dec_A) * cos(dec_B) * pow(sin((ra_B - ra_A)/2), 2)  )));
}

void crossmatch_in_radian(DatumList *reference, DatumList *samples, double distance_max) {
    Datum refDatum;
    Datum splDatum;
    const double RAD_DISTANCE = distance_max * (PI/180);
    int i, j;
    double c;

    int count = 0;
    printf("hello radian\n");
    for (i=0; i<reference->size; i++) {
        refDatum = reference->datums[i];
        for (j=0; j<samples->size; j++) {

            c = haversine(refDatum.raRad, refDatum.decRad,
                          splDatum.raRad, splDatum.decRad);
            
            if (c < RAD_DISTANCE)
                count++;
        }
    }
    printf("count is %i\n",count);
    return;
}

void crossmatch_run(DatumList *reference, 
                    DatumList *samples, double distance_max) {
    crossmatch_bad(reference, samples); /* wrong */
    crossmatch_all_spherical(reference, samples, distance_max); /* wrong */
    crossmatch_in_radian(reference, samples, distance_max); /* good */
}

