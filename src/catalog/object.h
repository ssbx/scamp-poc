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

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdbool.h>

typedef struct {
    unsigned long long id; /* unique identifier */
    double ra;      /* right assention (rad) (x) */
    double dec;     /* declinaison     (rad) (y) */
    double orthoSD; /* ortho standard deviation */
    double decSD;   /* declinaison standard deviation? */
    double sd;      /* maximum standard deviation? */
} Object_T;


/*
 * Create and initialize an Object structure. Inputs are in degree,
 * and transformed in radians.
 */
extern Object_T Object_new(
        unsigned long long id,
        double ra,
        double dec,
        double orthoSD,
        double decSD);

/*
 * Determine if the two objects position fit into the "limit" distance.
 * "factor" should be set to 1.0f by default. Increase and decrease
 * it to influence the match.
 */
extern bool Object_areClose(
        Object_T a,
        Object_T b,
        double factor);

/*
 * Return true if the two objects may
 */
extern bool Object_canMatchInRa(Object_T a, Object_T b);

extern bool Object_canMatchInDec(Object_T a, Object_T b);

#endif /* __OBJECT_H__ */
