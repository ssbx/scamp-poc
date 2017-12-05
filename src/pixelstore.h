/*
 * Healpix pixels storage mechanism.
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


#ifndef SRC_PIXELSTORE_H_
#define SRC_PIXELSTORE_H_

#include "catalog.h"

typedef enum {
    STORE_SCHEME_AVLTREE, /* fast 0(log n) and can growth to big nsides */
    STORE_SCHEME_BIGARRAY /* extra fast 0(1), but require a LOT of memory */
} StoreScheme;

/**
 * HealpixCell store pointers to every objects of all fields, belonging to a
 * common healpix pixel.
 */
typedef struct HealPixel {

    long id;            /* healpix id */
    Object **objects;   /* our objects pointers */
    int nobjects;       /* number of objects belonging to this pixel */
    int size;           /* for reallocation if required */
    long neighbors[8];  /* Neighbors indexes */

} HealPixel;

typedef struct PixelStore {
    void        *pixels; /* our opaque data */
    StoreScheme  scheme; /* determinate what is in our opaque data */
} PixelStore;


extern PixelStore*
PixelStore_new(Field *fields, int nfields, long nsides);

extern HealPixel*
PixelStore_get(PixelStore *store, long key);

extern void
PixelStore_free(PixelStore *store);

#endif /* SRC_PIXELSTORE_H_ */
