/*
 * Read and load data from files to object structures.
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


#include <string.h>
#include <fitsio.h>
#include <math.h>
#include <chealpix.h>

#include "catalog.h"
#include "mem.h"
#include "logger.h"

static char* read_field_card(fitsfile*,int*,char*);
static void insert_object_in_zone(Object*, ObjectZone*);

void
Catalog_open(char *filename, Field *field, long nsides) {
    fitsfile *fptr;
    int i, j, k, l;
    int status, ncolumns, nhdus, hdutype, nkeys, nwcsreject, nwcs, npix;
    long nrows;
    char *field_card, *charnull;
    struct wcsprm *wcs;
    Set set;

    // short shortnull;
    int   anynull;
    long  longnull;
    float floatnull;

    // shortnull = 0;
    status      = 0;
    longnull    = 0;
    floatnull   = 0.0;
    charnull    = ALLOC(sizeof(char) * 2); strcpy(charnull, " ");



    if (fits_open_file(&fptr, filename, READONLY, &status)) {
        if (status) {
            Logger_log(LOGGER_CRITICAL,
                    "Open FITS file failed with status %i\n", status);
        }
    }

    if (fits_get_num_hdus(fptr, &nhdus, &status)) {
        if (status) {
            Logger_log(LOGGER_CRITICAL,
                    "Read FITS HDUs number failed with status %i\n", status);
        }
    }

    npix = nside2npix(nsides);
    Logger_log(LOGGER_DEBUG,
            "Will divide sphere into %li parts for file %s\n",
            npix, filename);

    /*
     * We are ignoring the first "standard" HDU
     */
    nhdus--;
    field->sets = (Set*) ALLOC(sizeof(Set) * nhdus / 2);
    field->nsets = nhdus / 2;

    /*
     * HDUS starts at 1, but we are ignoring it (standard HDU) . So start at 2.
     * We are reading sextractor catalog which store a "Field header card" as
     * a single row table, containing original image informations. We need it
     * for WCS.
     *
     * So:
     * - even hdus contains a FITS hdu string about the following...
     * - ... odd hdus containing a FITS LDAC table.
     */

    for (i=2, l=0; i <= nhdus; i+=2, l++) {

        Logger_log(LOGGER_TRACE, "Reading fits file %s\n", filename);

        /*
         * even hdu contain original image FITS header
         */
        fits_movabs_hdu(fptr, i, &hdutype, &status);
        field_card = read_field_card(fptr, &nkeys, charnull);

        /*
         * create wcsprm with the image FITS header
         */
        status = wcsbth(field_card, nkeys, WCSHDR_all, 0, 0, NULL,
                            &nwcsreject, &nwcs, &wcs);

        if (status != 0)
            Logger_log(LOGGER_CRITICAL,
                    "Can not read WCS in sextractor field card\n");


        Logger_log(LOGGER_TRACE,
                "Number of WCS coordinate representations: %i with naxis %i\n",
                nwcs, wcs[0].naxis);

        /*
         * Now we should have required informations in "struct wcsprm *wcs".
         */
        FREE(field_card);

        /*
         * Move to the next HDU, witch is the data table.
         */
        fits_movabs_hdu(fptr, i+1, &hdutype, &status);

        /*
         * Dump table and apply WCS transformation on objects.
         */
        fits_get_num_cols(fptr, &ncolumns, &status);
        if (ncolumns != 21)
            Logger_log(LOGGER_CRITICAL, "Error: this HDU is not sextractor table\n");

        fits_get_num_rows(fptr, &nrows, &status);
        Logger_log(LOGGER_TRACE, "Have %i rows in the table\n", nrows);


        /*
         * Now begin to load column values.
         */

        /* Get "number" row */
        long *col_number = ALLOC(sizeof(long) * nrows);
        fits_read_col(fptr, TLONG, 1, 1, 1, nrows, &longnull,  col_number,
                &anynull, &status);

        /* Get "x_image" row */
        float *x_image = ALLOC(sizeof(float) * nrows);
        fits_read_col(fptr, TFLOAT, 2, 1, 1, nrows, &floatnull,  x_image,
                &anynull, &status);

        /* Get "y_image" row */
        float *y_image = ALLOC(sizeof(float) * nrows);
        fits_read_col(fptr, TFLOAT, 3, 1, 1, nrows, &floatnull,  y_image,
                &anynull, &status);

        /* Use these if needed later */
//
//        /* Get "xwin_image" row */
//        double *xwin_image = ALLOC(sizeof(double) * nrows);
//        fits_read_col(fptr, TDOUBLE, 4, 1, 1, nrows, &floatnull,  xwin_image,
//                &anynull, &status);
//
//        /* Get "ywin_image" row */
//        double *ywin_image = ALLOC(sizeof(double) * nrows);
//        fits_read_col(fptr, TDOUBLE, 5, 1, 1, nrows, &floatnull,  ywin_image,
//                &anynull, &status);
//
//        /* Get "erra_image" row */
//        float *erra_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 6, 1, 1, nrows, &floatnull,  erra_image,
//                &anynull, &status);
//
//        /* Get "errb_image" row */
//        float *errb_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 7, 1, 1, nrows, &floatnull,  errb_image,
//                &anynull, &status);
//
//        /* Get "errtheta_image" row */
//        float *errtheta_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 8, 1, 1, nrows, &floatnull,  errtheta_image,
//                &anynull, &status);
//
//        /* Get "errawin_image" row */
//        float *errawin_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 9, 1, 1, nrows, &floatnull,  errawin_image,
//                &anynull, &status);
//
//        /* Get "errbwin_image" row */
//        float *errbwin_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 10, 1, 1, nrows, &floatnull,  errbwin_image,
//                &anynull, &status);
//
//        /* Get "errthetawin_image" row */
//        float *errthetawin_image = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 11, 1, 1, nrows, &floatnull,  errthetawin_image,
//                &anynull, &status);
//
//        /* Get "flux_auto" row */
//        float *flux_auto = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 12, 1, 1, nrows, &floatnull,  flux_auto,
//                &anynull, &status);
//
//        /* Get "fluxerr_auto" row */
//        float *fluxerr_auto = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 13, 1, 1, nrows, &floatnull,  fluxerr_auto,
//                &anynull, &status);
//
//        /* Get "flux_radius" row */
//        float *flux_radius = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 14, 1, 1, nrows, &floatnull,  flux_radius,
//                &anynull, &status);
//
//        /* Get "flags" row */
//        short *flags = ALLOC(sizeof(short) * nrows);
//        fits_read_col(fptr, TSHORT, 15, 1, 1, nrows, &shortnull,  flags,
//                &anynull, &status);
//
//        /* Get "x_world" row */
//        double *x_world = ALLOC(sizeof(double) * nrows);
//        fits_read_col(fptr, TDOUBLE, 16, 1, 1, nrows, &floatnull,  x_world,
//                &anynull, &status);
//
//        /* Get "y_world" row */
//        double *y_world = ALLOC(sizeof(double) * nrows);
//        fits_read_col(fptr, TDOUBLE, 17, 1, 1, nrows, &floatnull,  y_world,
//                &anynull, &status);
//
//        /* Get "erra_world" row */
//        float *erra_world = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 18, 1, 1, nrows, &floatnull,  erra_world,
//                &anynull, &status);
//
//        /* Get "errb_world" row */
//        float *errb_world = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 19, 1, 1, nrows, &floatnull,  errb_world,
//                &anynull, &status);
//
//        /* Get "errtheta_world" row */
//        float *errtheta_world = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 20, 1, 1, nrows, &floatnull,  errtheta_world,
//                &anynull, &status);
//
//        /* Get "mag_auto" row */
//        float *mag_auto = ALLOC(sizeof(float) * nrows);
//        fits_read_col(fptr, TFLOAT, 21, 1, 1, nrows, &floatnull,  mag_auto,
//                &anynull, &status);


        /*
         * WCS transformation
         */
        double *pixcrd, *imgcrd, *phi, *theta, *world;
        int *stat;
        pixcrd  = ALLOC(sizeof(double) * nrows * 2);
        imgcrd  = ALLOC(sizeof(double) * nrows * 2);
        phi     = ALLOC(sizeof(double) * nrows * 2);
        theta   = ALLOC(sizeof(double) * nrows * 2);
        world   = ALLOC(sizeof(double) * nrows * 2);
        stat    = CALLOC(sizeof(int), nrows * 2);

        for (j=0, k=0; j < nrows; j++, k+=2) {
            pixcrd[k]   = x_image[j];
            pixcrd[k+1] = y_image[j];
        }


        wcsp2s(wcs, nrows, 2, pixcrd, imgcrd, phi, theta, world, stat);


        for (j=0; j < nrows * 2; j++) {
            if (stat[j] != 0) {
                Logger_log(LOGGER_ERROR, "ERROR %i: for %i\n", stat[j], j);
            }
        }

        Logger_log(LOGGER_TRACE, "File %s read. Create object set\n", filename);


        /*
         * Create a set of objects (a CCD)
         */
        set.objects = ALLOC(sizeof(Object) * nrows);
        set.nobjects = nrows;
        set.wcs = wcs;
        set.nwcs = nwcs;
        set.field = field;
        Object obj;
        for (j=0, k=0; j < nrows; j++, k+=2) {

            obj.id      = col_number[j];
            obj.raDeg   = world[k];
            obj.decDeg  = world[k+1];
            obj.ra      = world[k]   * M_PI/180;
            obj.dec     = world[k+1] * M_PI/180;

            ang2pix_ring(nsides, obj.dec, obj.ra, &obj.ipring);

            set.objects[j] = obj;
            obj.set = &field->sets[l];

        }


        /*
         * Add it in the field structure
         */
        field->sets[l] = set;

        FREE(col_number);
        FREE(x_image);
        FREE(y_image);

        FREE(pixcrd);
        FREE(imgcrd);
        FREE(phi);
        FREE(theta);
        FREE(world);
        FREE(stat);

    }

    fits_close_file(fptr, &status);

}


void
Catalog_freefield(Field *field) {
    int i;
    for (i=0; i<field->nsets; i++) {
        FREE(field->sets[i].objects);
        wcsvfree(&field->sets[i].nwcs, &field->sets[i].wcs);
    }
    FREE(field->sets);
}


void
Catalog_dump(Field *field) {
    int i, j;
    Object obj;
    for (i=0; i<field->nsets; i++) {
        for (j=0; j<field->sets[i].nobjects; j++) {
            obj = field->sets[i].objects[j];
            printf("ra: %f dec: %f num: %li\n", obj.ra, obj.dec, obj.id);
        }
    }
}

ObjectZone *
Catalog_initzone(long nsides) {
	ObjectZone *zones, zone;
	long npix, i;
	npix = nside2npix(nsides);

	Logger_log(LOGGER_DEBUG,
			"Will allocate room for zones. It will take %i MB\n",
			sizeof(ObjectZone) * npix / 1000000);

	zones = (ObjectZone*) ALLOC(sizeof(ObjectZone) * npix);

	/* XXX omp parallel */
	zone.objects = NULL;
	zone.nobjects = 0;
	zone.size = 0;
	for (i=0; i<npix; i++) {
		zones[i] = zone;
	}
	return zones;
}

static int cmp_objects_on_ra(const void *a, const void *b) {
	Object **aa, **bb;
	aa = (Object**) a;
	bb = (Object**) b;
	return (*aa)->ra == (*bb)->ra ? 0 : ((*aa)->ra < (*bb)->ra ? -1 : 1);
}

void
Catalog_fillzone(Field *fields, int nfields, ObjectZone *zones, long nsides) {
	int i, j, k;
	Field *field;
	Set *set;
	Object *obj;
	ObjectZone *z;
	long total_nobjects;
	total_nobjects = 0;
	for (i=0; i<nfields; i++) {
		field = &fields[i];
		for (j=0; j<field->nsets; j++) {
			set = &field->sets[j];
			total_nobjects += set->nobjects;
			for (k=0; k<set->nobjects; k++) {

				obj  = &set->objects[k];
				insert_object_in_zone(obj, &zones[obj->ipring]);

			}
		}
	}

	Logger_log(LOGGER_DEBUG,
			"Total size for zones is %li MB\n",
			(nside2npix(nsides) * sizeof(ObjectZone) +
					total_nobjects * sizeof(Object*)) / 1000000);
	/*
	 * Reallocate to save some room,
	 * And sort by right ascension.
	 */
	for (i=0; i<nside2npix(nsides);i++) {
		z = &zones[i];
		if (z->objects == NULL)
			continue;
		z->objects = REALLOC(z->objects, sizeof(ObjectZone*) * z->nobjects);
		Logger_log(LOGGER_TRACE,
				"Have %i matches for zone %i\n",
				z->nobjects, i);
		qsort(z->objects, z->nobjects, sizeof(Object*), cmp_objects_on_ra);
	}
}

void
Catalog_freezone(ObjectZone *zones, long nsides) {
	int i;
	for (i=0; i<nside2npix(nsides);i++) {
		FREE(zones[i].objects);
	}
	FREE(zones);
}

/*
 * Static utilities
 */
#define ZONE_BASE_SIZE 100
static void
insert_object_in_zone(Object *obj, ObjectZone *z) {

	/*
	 * TODO ObjectZone.objects should be a pointer to an array
	 * (where index refer to fields) of array of objects.
	 */
    /* First allocation */
    if (z->objects == NULL) {

        z->objects = ALLOC(sizeof(Object*) * ZONE_BASE_SIZE);
        z->size = ZONE_BASE_SIZE;
        z->nobjects = 0;

    } else if (z->size == z->nobjects) { /* need realloc */

        z->objects = REALLOC(z->objects, sizeof(Object*) * z->size * 2);
        z->size *= 2;

    }

    /* append object */
    z->objects[z->nobjects] = obj;
    z->nobjects++;

}

static char*
read_field_card(fitsfile *fptr, int *nkeys, char *charnull) {
    int status = 0, anynull = 0, field_card_size, charpos, i;
    char *field_card;
    char *buff;

    fits_get_col_display_width(fptr, 1, &field_card_size, &status);
    if (status) {
        fits_report_error(stderr, status);
        exit(status);
    }

    field_card = ALLOC(sizeof(char) * field_card_size);

    *nkeys = field_card_size / 80;

    /*
     * XXX this is a hack. Fitsio do not know how to read this single column
     * single row single element data, but accept to increment the element count.
     */
    memset(field_card, ' ', field_card_size);
    for (i=0, charpos=0; i<*nkeys; i++,charpos+=80) {
        buff = &field_card[charpos];
        fits_read_col(
              fptr, TSTRING, 1, 1, 1+i, 1, &charnull, &buff, &anynull, &status);

    }

    return field_card;
}
