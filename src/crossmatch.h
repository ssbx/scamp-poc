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

typedef struct Matches {

} Matches;

/**
 * HealpixCell store pointers to every objects of a field, belonging to a
 * common healpix pixel.
 */
typedef struct HealpixCell {

    Object  **objects;  /* our pointers */
    int     nobjects;   /* number of pointer */
    int     size;       /* for reallocation if required */

    long neighbors[8];

} HealpixCell;

/*
 * Cross match fields with filed HealpixCell's.
 */
extern void
Crossmatch_crossCells(HealpixCell **zones, long *zoneindex,
                        long nzoneindex, double radius);
/**
 * Free all memory allocated for a zone.
 *
 * Thread safe.
 */
extern void
Crossmatch_freeCells(HealpixCell **zones, long nsides);

/**
 * Allocate memory for the key/value pixels store (ObjectZone).
 * Must be freed by Catalog_freezone/2
 */
extern HealpixCell **
Crossmatch_initCells(long nsides);

/**
 * Return an array of pixel id used by the fields in the nested scheme.
 * Set "nzone" to the size of the array. Array must be freed by the user.
 */
extern long*
Crossmatch_fillCells(Field *fields, int nfields,
                    HealpixCell **cells, long nsides, long *nzone);

#endif /* __CROSSMATCH_H__ */
