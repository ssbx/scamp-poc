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
#include "object.h"

/* 
 * Get the distance between two angles. Inputs and output
 * are in radians.
 *
 * See https://fr.wikipedia.org/wiki/Formule_de_haversine 
 * for the algorithm.
 */
static inline double haversine(double  ra_A, 
                               double dec_A,
                               double  ra_B, 
                               double dec_B) {

    /* Why???
    double distance;
    double term;

    term = sqrt(
        pow(sin( (dec_B - dec_A) * 0.5) , 2) +
        cos(dec_A) * cos(dec_B) *
        pow(sin((ra_B - ra_A) * 0.5) , 2)
    );

    if (term >= 1.0f)
        distance = M_PI;
    else
        distance = 2 * asin(term);
    return distance;
    */
    return (2 * asin(sqrt(
        pow(sin( (dec_B - dec_A) * 0.5) , 2) +
        cos(dec_A) * cos(dec_B) *
        pow(sin((ra_B - ra_A) * 0.5) , 2)
    )));

}

/*
 * Take two ObjectList and count the number of match
 * between them.
 */
void crossmatch_run(ObjectList *reference, 
                    ObjectList *samples, 
                    double      distance_max) {
    Object refObject;
    Object splObject;
    distance_max = distance_max * M_PI/180;
    int i, j;
    double distance;

    int count = 0;
    printf("hello radian\n");
    for (i=0; i<reference->size; i++) {
        refObject = reference->objects[i];

        for (j=0; j<samples->size; j++) {
            splObject = samples->objects[j];
            
            distance = haversine(refObject.ra, refObject.dec,
                                 splObject.ra, splObject.dec);

            if (distance < distance_max)
                count++;
        }
    }
    printf("count is %i\n",count);
    return;
}
