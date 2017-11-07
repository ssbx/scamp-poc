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

extern Object_T Object_new(
        unsigned long long id,
        double ra,
        double dec,
        double orthoSD,
        double decSD);

extern bool Object_areClose(
        Object_T a,
        Object_T b,
        double factor);

#endif /* __OBJECT_H__ */
