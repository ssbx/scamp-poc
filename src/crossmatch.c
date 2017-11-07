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

static inline double haversine(double ra_A, double dec_A,
                               double ra_B, double dec_B) {
    /* see https://fr.wikipedia.org/wiki/Formule_de_haversine */
    return (2 * asin(sqrt(
        pow(sin( (dec_B - dec_A) / 2) , 2) +
        cos(dec_A) * cos(dec_B) *
        pow(sin((ra_B - ra_A) / 2) , 2)
    )));
}

void crossmatch_in_radian(DatumList *reference, DatumList *samples, double distance_max) {
    Datum refDatum;
    Datum splDatum;
    const double RAD_DISTANCE = distance_max * (M_PI/180);
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
    crossmatch_in_radian(reference, samples, distance_max); /* good */
}

