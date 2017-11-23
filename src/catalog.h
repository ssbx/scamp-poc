/*
 * Load and manipulate and perform actions on Objects common to a
 * single catalog.
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

#ifndef __CATALOG_H__
#define __CATALOG_H__

#include <wcslib/wcshdr.h>

/**
 * Object structure represent a set entry. ra and dec are both represented
 * in degree (for wcslib) and radians (for healpix).
 */
typedef struct Object {

    long    id;     /* same as "number" in sextractor catalog */
    double  ra;     /* right ascension (rad) (x) world coordinates */
    double  dec;    /* declination     (rad) (y) world coordinates */

    double raDeg;   /* ra in degree */
    double decDeg;  /* dec in degree */

    long ipring;   /* position on healpix ring */

} Object;

/**
 * Set's objects share a common image source (a CCD).
 */
typedef struct Set {

    Object *objects;
    int     nobjects;

    /*
     * wcsprm structures used by wcslib to convert from pixel to world
     * coordinates and vice versa. (see wcsp2s and wcss2p)
     */
    struct wcsprm *wcs;
    int nwcs;

} Set;

/**
 * A field represent a file containing Set(s).
 */
typedef struct Field {

    Set *sets;
    int  nsets;

} Field;

/**
 * Open a catalog. Presently only support sextractor catalogs. The Field
 * structure given in input must be freed by the user with Catalog_free().
 */
void Catalog_open(char *file, Field *field);

/**
 * Print the content of catalogs. Used for debugging purpose.
 */
void Catalog_dump(Field *field);

/**
 * Free all memory allocated for this field.
 */
void Catalog_free(Field *field);

#endif /* __CATALOG_H__ */
