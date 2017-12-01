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

struct Object;
struct Set;
struct Field;


/**
 * Object structure represent a set entry. ra and dec are both represented
 * in degree (for wcslib), radiant and vectors (for healpix).
 */
typedef struct Object {

    /* same as "number" in sextractor catalog */
    long    id;

    /* Default unit is radiant, used by healpix */
    double  ra;     /* right ascension (rad) (x) world coordinates */
    double  dec;    /* declination     (rad) (y) world coordinates */

    /* Degrees used by WCS */
    double raDeg;   /* ra in degree */
    double decDeg;  /* dec in degree */

    /* Representation as vector used to determinate the angle distance
     * with another vector in the cross-match algorithm (see angdist) */
    double vector[3];

    /* position on healpix nested scheme */
    long pix_nest;

    /* Object belonging to this set */
    struct Set *set;

    /* Best matching object from another field */
    struct Object *bestMatch;

    /* Best distance is the distance to bestMatch in radiant. It is initialized
     * in the Crossmatch_crossSeel function to the value of max radius and used
     * in the cross matching algorithm. May contain a value (initial radius)
     * without a bestMatch object
     */
    double bestMatchDistance;

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

    struct Field *field;

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
 *
 * Tread safe.
 */
extern void
Catalog_open(char *file, Field *field);

/**
 * Print the content of catalogs. Used for debugging purpose.
 *
 * Thread safe.
 */
extern void
Catalog_dump(Field *field);

/**
 * Free all memory allocated for a field.
 *
 * Thread safe.
 */
extern void
Catalog_freeField(Field *field);



#endif /* __CATALOG_H__ */
