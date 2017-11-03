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

#ifndef __DATUM_H__
#define __DATUM_H__

typedef struct {
    unsigned long long id;
    double ra;      // right assention  (x)
    double dec;     // declinaison       (y)
    double orthoSD; // ortho standard deviation
    double decSD;   // declinaison standard deviation?
    double sd;      // maximum standard deviation?
} Datum;

Datum datum_create(
        unsigned long long id,
        double ra,
        double dec,
        double orthoSD,
        double decSD);

#endif // __DATUM_H__
