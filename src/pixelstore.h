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

#include <stdbool.h>
#include "scamp.h"

typedef struct HealPixel HealPixel;
/**
 * HealPixel store pointers to every samples of all fields, belonging to a
 * common healpix pixel.
 */
struct HealPixel {

    long id;            /* healpix id */
    Sample *samples;    /* our samples */
    Sample ***ext;       /* update ext if realloc is called */
    int nsamples;       /* number of samples belonging to this pixel */
    int size;           /* for reallocation if required */
    int64_t neighbors[8];  /* Neighbors indexes */
    HealPixel *pneighbors[8];
    bool tneighbors[8]; /* check if neighbors have allready been matched */

};

typedef struct PixelStore {
    int64_t     nsides;
    void        *pixels; /* our opaque data */

    /* These are used to iterate over pixels */
    long        npixels;
    int64_t     *pixelids;
    int         pixelids_size; /* PRIVATE, for re allocation if required */

} PixelStore;


extern PixelStore*
PixelStore_new(int64_t nsides);

/* 
 * Store "spl" in "store" in pixel id "key". Set "ext to contains a pointer to
 * our newly created sample. In case of internal reallocation, ext is 
 * updated automatically.
 */
extern void
PixelStore_add(PixelStore *store, Sample spl, Sample **ext);

extern HealPixel*
PixelStore_get(PixelStore *store, int64_t key);

extern void
PixelStore_free(PixelStore *store);

extern void
PixelStore_setMaxRadius(PixelStore *store, double radius);
#endif /* SRC_PIXELSTORE_H_ */
