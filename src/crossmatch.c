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
#include "objects/object.h"

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

    int count = 0;
    printf("hello radian\n");
    for (i=0; i<reference->size; i++) {
        refObject = reference->objects[i];

        for (j=0; j<samples->size; j++) {
            splObject = samples->objects[j];
            
            if (object_areClose(refObject, splObject, distance_max))
                count++;

        }
    }

    printf("count is %i\n",count);
    return;
}
