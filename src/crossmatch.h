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

#ifndef __CROSSMATCH_H__
#define __CROSSMATCH_H__

#include "catalog.h"
#include "pixelstore.h"

typedef enum {
    ALGO_NEIGHBORS,
    ALGO_QUERYDISC
} CrossmatchAlgo;


extern void
Crossmatch_crossFields(Field *fields, int nfields, long nsides,
        double radius_arcsec, CrossmatchAlgo algo, StoreScheme scheme);



#endif /* __CROSSMATCH_H__ */
