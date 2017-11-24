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

extern long Catalog_SIDES;
struct Set;
struct Field;

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

    long ipring;    /* position on healpix ring */

    struct Set *set;


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
 * ObjectZone store pointers to every objects of a field, belonging to a
 * common zone.
 */
typedef struct ObjectZone {

    Object **objects;   /* our pointers */
    int     nobjects;   /* number of pointer */
    int     size;       /* for reallocation if required */

} ObjectZone;

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
extern void Catalog_open(char *file, Field *field, long nsides);

/**
 * Print the content of catalogs. Used for debugging purpose.
 *
 * Thread safe.
 */
extern void Catalog_dump(Field *field);

/**
 * Free all memory allocated for a field.
 *
 * Thread safe.
 */
extern void Catalog_freefield(Field *field);

/**
 * Free all memory allocated for a zone.
 *
 * Thread safe.
// */
//extern void Catalog_freezone(ObjectZone *zone, nside);

extern ObjectZone *Catalog_initzone(long nsides);
extern void Catalog_fillzone(Field *fields, int nfields, ObjectZone *zones, long nsides);
extern void Catalog_freezone(ObjectZone *zones, long nsides);

#endif /* __CATALOG_H__ */
