/*
 * Read and load data from files to samples structures.
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
#include <cfitsio/fitsio.h>
#include <chealpix.h>

#include "catalog.h"
#include "mem.h"
#include "logger.h"

static char* read_field_card(fitsfile*,int*,char*);
static char charnull[2] = {' ', '\0'};


void
Catalog_open(char *filename, Field *field, PixelStore *store) {
    fitsfile *fptr;
    int i, j, k, l;
    int status, ncolumns, nhdus, hdutype, nkeys, nwcsreject, nwcs;
    long nrows;
    char *field_card;
    struct wcsprm *wcs;

    // short shortnull;
    int   anynull;
    long  longnull;
    float floatnull;

    // shortnull = 0;
    status      = 0;
    longnull    = 0;
    floatnull   = 0.0;


    if (fits_open_file(&fptr, filename, READONLY, &status)) {
        if (status) {
            Logger_log(LOGGER_CRITICAL,
                    "Open FITS file %s failed with status %i\n", filename, status);
        }
    }

    if (fits_get_num_hdus(fptr, &nhdus, &status)) {
        if (status) {
            Logger_log(LOGGER_CRITICAL,
                    "Read FITS HDUs number failed with status %i\n", status);
        }
    }


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

        Logger_log(LOGGER_TRACE, "Reading fits HDU %s %i\n", filename, i);

        /*
         * even hdu contain original image FITS header
         */
        fits_movabs_hdu(fptr, i, &hdutype, &status);
        field_card = read_field_card(fptr, &nkeys, charnull);

        Logger_log(LOGGER_TRACE, "Read fieldcard %s %i\n", field_card, strlen(field_card));
        /*
         * create wcsprm with the image FITS header
         */
        status = wcsbth(field_card, nkeys, WCSHDR_all, 0, 0, NULL,
                            &nwcsreject, &nwcs, &wcs);

        if (status != 0)
            Logger_log(LOGGER_CRITICAL,
                    "Can not read WCS in sextractor field card\n");

        Logger_log(LOGGER_TRACE, "Reading fits hav successfuly applyed wcsbth %s\n", filename);
        Logger_log(LOGGER_TRACE,
                "Number of WCS coordinate representations: %i with naxis %i\n",
                nwcs, wcs[0].naxis);

        /*
         * Now we should have required informations in "struct wcsprm *wcs".
         */
        Logger_log(LOGGER_TRACE, "Reading fits ffffffffffffffff %s\n", filename);

        FREE(field_card);

        /*
         * Move to the next HDU, witch is the data table.
         */
        fits_movabs_hdu(fptr, i+1, &hdutype, &status);

        /*
         * Dump table and apply WCS transformation on samples.
         */
        fits_get_num_cols(fptr, &ncolumns, &status);

        fits_get_num_rows(fptr, &nrows, &status);
        Logger_log(LOGGER_TRACE, "Have %i rows and %i cols in the table\n", nrows, ncolumns);

        if (nrows <= 0) {
            Logger_log(LOGGER_ERROR, "file %i hdu %i contain an empty table\n", filename, i);
            continue;
        }

        /*
         * Now begin to load column values.
         */

        int num_col, x_image_col, y_image_col;
        fits_get_colnum(fptr, CASESEN, "NUMBER", &num_col, &status);
        fits_get_colnum(fptr, CASESEN, "X_IMAGE", &x_image_col, &status);
        fits_get_colnum(fptr, CASESEN, "Y_IMAGE", &y_image_col, &status);

        /* Get "number" row */
        long *col_number = ALLOC(sizeof(long) * nrows);
        fits_read_col(fptr, TLONG, num_col, 1, 1, nrows, &longnull,  col_number,
                &anynull, &status);

        /* Get "x_image" row */
        float *x_image = ALLOC(sizeof(float) * nrows);
        fits_read_col(fptr, TFLOAT, x_image_col, 1, 1, nrows, &floatnull,  x_image,
                &anynull, &status);

        /* Get "y_image" row */
        float *y_image = ALLOC(sizeof(float) * nrows);
        fits_read_col(fptr, TFLOAT, y_image_col, 1, 1, nrows, &floatnull,  y_image,
                &anynull, &status);

        /*
         * WCS transformation
         */
        double *pixcrd, *imgcrd, *phi, *theta, *world;
        int *stat;
        pixcrd  = ALLOC(sizeof(double) * nrows * 3);
        imgcrd  = ALLOC(sizeof(double) * nrows * 2);
        phi     = ALLOC(sizeof(double) * nrows * 2);
        theta   = ALLOC(sizeof(double) * nrows * 2);
        world   = ALLOC(sizeof(double) * nrows * 2);
        stat    = CALLOC(sizeof(int), nrows * 2);

        for (j=0, k=0; j < nrows; j++, k+=3) {
            pixcrd[k] = 0;
            pixcrd[k+1]   = x_image[j];
            pixcrd[k+2] = y_image[j];
        }


        wcsp2s(wcs, nrows, 2, pixcrd, imgcrd, phi, theta, world, stat);

        for (j=0; j < nrows * 2; j++) {
            if (stat[j] != 0) {
                Logger_log(LOGGER_ERROR, "ERROR %i: for %i\n", stat[j], j);
            }
        }

        Logger_log(LOGGER_TRACE, "File %s read. Create samples \n", filename);

        /*
         * Create a set of samples (a CCD)
         */
        field->sets[l].samples = ALLOC(sizeof(Sample*) * nrows);
        field->sets[l].nsamples = nrows;
        field->sets[l].wcs = wcs;
        field->sets[l].nwcs = nwcs;
        field->sets[l].field = field;

        Sample sample;
        for (j=0, k=0; j < nrows; j++, k+=2) {

            sample.id   = col_number[j];
            sample.ra   = world[k];
            sample.dec  = world[k+1];
            sample.lon  = world[k] * TO_RAD;
            /* degree latitude to radian colatitude */
            sample.col     = SC_HALFPI - world[k+1] * TO_RAD;
            sample.set     = &field->sets[l];
            PixelStore_add(store, sample, &field->sets[l].samples[j]);
        }

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
Catalog_freeField(Field *field) {
    int i;
    for (i=0; i<field->nsets; i++) {
        FREE(field->sets[i].samples);
        wcsvfree(&field->sets[i].nwcs, &field->sets[i].wcs);
    }
    FREE(field->sets);
}


void
Catalog_dump(Field *field) {
    int i, j;
    Sample *sample;
    for (i=0; i<field->nsets; i++) {
        for (j=0; j<field->sets[i].nsamples; j++) {
            sample = field->sets[i].samples[j];
            printf("ra: %f dec: %f num: %li\n", sample->lon, sample->col, sample->id);
        }
    }
}

static char*
read_field_card(fitsfile *fptr, int *nkeys, char *charnull) {
    int status = 0, anynull = 0, field_card_size, charpos, i;
    char *field_card;
    char *buff;

    fits_get_col_display_width(fptr, 1, &field_card_size, &status);
    int newsize;
    long nn;
    fits_read_tdim(fptr, 1, 1, &newsize, &nn, &status);

    if (status) {
        fits_report_error(stderr, status);
        exit(status);
    }

    field_card = ALLOC(sizeof(char) * field_card_size);
    *nkeys = field_card_size / 80;

    Logger_log(LOGGER_TRACE, "fieldcard size %i, nkeys %i\n", field_card_size, *nkeys);

    /*
     * XXX this is a hack. Fitsio do not know how to read this single column
     * single row single element data, but accept to increment
     * the element count.
     */
    for (i=0, charpos=0; i<*nkeys; i++,charpos+=80) {
        buff = &field_card[charpos];
        fits_read_col(
              fptr, TSTRING, 1, 1, 1+i, 1, &charnull, &buff, &anynull, &status);

    }
    field_card[field_card_size] = '\0';

    return field_card;
}

void
test_Catalog_open_ascii(char *filename, Field *field, PixelStore *store) {
    FILE *fp = fopen(filename, "r");
    int set_size = 10;

    /* This is a single set file */
    field->nsets = 1;
    field->sets = ALLOC(sizeof(Set));

    Set set;
    set.nsamples = 0;
    set.samples = ALLOC(sizeof(Sample*) * set_size);
    set.field = field;

    Sample spl;
    spl.set = &field->sets[0];
    while (fscanf(fp, "%li %lf %lf\n", &spl.id, &spl.lon, &spl.col) > 0) {
        if (set.nsamples == set_size) {
            set.samples = REALLOC(set.samples, sizeof(Sample*) * set_size * 2);
            set_size *= 2;
        }
        PixelStore_add(store, spl, &set.samples[set.nsamples]);
        set.nsamples++;
    }

    field->sets[0] = set;

    fclose(fp);
}

