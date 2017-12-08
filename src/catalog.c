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
#include <fitsio.h>
#include <chealpix.h>

#include "catalog.h"
#include "mem.h"
#include "logger.h"

static char* read_field_card(fitsfile*,int*,char*);
static char charnull[2] = {' ', '\0'};

void
Catalog_open(char *filename, Field *field) {
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
                    "Open FITS file failed with status %i\n", status);
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
         * Dump table and apply WCS transformation on samples.
         */
        fits_get_num_cols(fptr, &ncolumns, &status);
        if (ncolumns != 21)
            Logger_log(LOGGER_CRITICAL,
                    "Error: this HDU is not sextractor table\n");

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

        Logger_log(LOGGER_TRACE, "File %s read. Create samples set\n", filename);

        /*
         * Create a set of samples (a CCD)
         */
        field->sets[l].samples = ALLOC(sizeof(Sample) * nrows);
        field->sets[l].nsamples = nrows;
        field->sets[l].wcs = wcs;
        field->sets[l].nwcs = nwcs;
        field->sets[l].field = field;

        Sample sample;
        for (j=0, k=0; j < nrows; j++, k+=2) {

            sample.id      = col_number[j];
            sample.raDeg   = world[k];
            sample.decDeg  = world[k+1];
            sample.ra      = world[k]   * SC_PI_DIV_180;
            sample.dec     = world[k+1] * SC_PI_DIV_180;
            sample.set     = &field->sets[l];

            field->sets[l].samples[j] = sample;

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
    Sample sample;
    for (i=0; i<field->nsets; i++) {
        for (j=0; j<field->sets[i].nsamples; j++) {
            sample = field->sets[i].samples[j];
            printf("ra: %f dec: %f num: %li\n", sample.ra, sample.dec, sample.id);
        }
    }
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
     * single row single element data, but accept to increment
     * the element count.
     */
    memset(field_card, ' ', field_card_size);
    for (i=0, charpos=0; i<*nkeys; i++,charpos+=80) {
        buff = &field_card[charpos];
        fits_read_col(
              fptr, TSTRING, 1, 1, 1+i, 1, &charnull, &buff, &anynull, &status);

    }

    return field_card;
}
