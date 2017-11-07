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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "crossmatch.h"
#include "objects/object.h"

/*
 * Take two ObjectList and count the number of match between them.
 */
void crossmatch_run(ObjectList *reference,
                    ObjectList *samples,
                    double      factor) {
    Object_T refObject;
    Object_T splObject;
    int i, j;

    int count = 0;

    for (i=0; i<reference->size; i++) {
        refObject = reference->objects[i];

        for (j=0; j<samples->size; j++) {
            splObject = samples->objects[j];

            if (Object_areClose(refObject, splObject, factor))
                count++;

        }
    }

    printf("count is %i\n",count);
    return;
}

