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
#include "datumlist.h"
#include "logger.h"
#include "fits/fitscat.h"


/*
 * Read a catalog with a very simple ASCII format.
 * Mainly used for testing.
 */
DatumList __read_ascii_file(char *fileName, int number) {
    FILE *file;
    DatumList dlist;
    unsigned long long id;
    double ra, orthoSD, dec, decSD;

    if ((file = fopen(fileName, "r")) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    datumlist_init(&dlist);
    while (fscanf(file, "%llu %lf %lf %lf %lf\n",
                &id, &ra, &orthoSD, &dec, &decSD) == 5) {
        datumlist_add(&dlist, datum_create(id, ra, orthoSD, dec, decSD));
    }
    return dlist;
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

    // TODO WARNING XXX free
    return catalog;
}


/*
 * Read several catalogs (FITS LDAC)
 */
catstruct** catalog_read_fitscat(char **inputFiles, int numInputFiles) {
    int i;
    catstruct **catalogs;
    catalogs = malloc(sizeof(catstruct*) * numInputFiles);

#pragma omp parallel for
    for (i=0; i < numInputFiles; i++) {
        catalogs[i] = __read_fitscat_file(inputFiles[i]);
    }

    // TODO WARNING XXX free
    return catalogs;

}

void catalog_free(catstruct **catalogs, int number) {
    free(catalogs);
}

/*
 * Read several catalogs (ASCII)
 */
void catalog_read_asciicat(char **inputFiles, int numInputFiles) {
    DatumList dlist;
    Datum d;
    int i, j;

    for (i=0; i < numInputFiles; i++) {
        dlist = __read_ascii_file(inputFiles[i], numInputFiles);
        for (j=0; j < dlist.size; j++) {
            d = dlist.datums[j];

            printf("id: %i %f %f %f %f\n", d.id, d.ra, d.dec, d.orthoSD, d.decSD);
        }
    }
}
