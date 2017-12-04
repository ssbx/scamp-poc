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


#ifndef SRC_CHEALPIXSPHERE_H_
#define SRC_CHEALPIXSPHERE_H_

#include "catalog.h"

/**
 * HealpixCell store pointers to every objects of a field, belonging to a
 * common healpix pixel.
 */
typedef struct HealPixel {

    Object  **objects;  /* our pointers */
    int     nobjects;   /* number of pointer */
    int     size;       /* for reallocation if required */

    long neighbors[8];

} HealPixel;


#endif /* SRC_CHEALPIXSPHERE_H_ */
