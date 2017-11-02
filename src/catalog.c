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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "datum.h"
#include "logger.h"
#include "fits/fitscat.h"


/*
 * Read a catalog with a very simple ASCII format.
 * Mainly used for testing.
 */
static void __read_ascii_file(char *fileName) {
    FILE *file;
    unsigned long long id;
    double ra, orthoSD, dec, decSD;
    Datum d;

    if ((file = fopen(fileName, "r")) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, "%llu %lf %lf %lf %lf\n",
                &id, &ra, &orthoSD, &dec, &decSD) == 5) {
        d = datum_create(id, ra, orthoSD, dec, decSD);
        logger_log(LOG_DEBUG, "%llu %lf %lf %lf %.10f <-\n",
                d.id, d.ra, d.orthoSD, d.dec, d.decSD);
    }
}

/*
 * Read a catalog of FITS LDAC format.
 */
static catstruct* __read_fitscat_file(char *fileName) {
    catstruct *catalog;

    if ((catalog = read_cat(fileName)) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return catalog;
}


/*
 * Read several catalogs (FITS LDAC)
 */
catstruct** catalog_read_fitscat(char **inputFiles, int numInputFiles) {
    catstruct **catalogs;
    int i;

    catalogs = malloc(sizeof(catstruct*) * numInputFiles);

#pragma omp parallel for
    for (i=0; i < numInputFiles; i++) {
        catalogs[i] = __read_fitscat_file(inputFiles[i]);
    }

    return catalogs;
}

/*
 * Read several catalogs (ASCII)
 */
void catalog_read_ascii(char **inputFiles, int numInputFiles) {
    int i;

#pragma omp parallel for
    for (i=0; i < numInputFiles; i++) {
        __read_ascii_file(inputFiles[i]);
    }
}
