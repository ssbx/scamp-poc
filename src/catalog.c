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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "object.h"
#include "objectlist.h"
#include "logger.h"
#include "fits/fitscat.h"


/*
 * Read a catalog with a very simple ASCII format. Mainly used for testing.
 * ObjectList must be freed with catalog_free_objects/1
 */
ObjectList catalog_read_ascii_file(char *fileName) {
    FILE *file;
    ObjectList dlist;
    unsigned long long id;
    double ra, orthoSD, dec, decSD;

    if ((file = fopen(fileName, "r")) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    objectlist_init(&dlist);
    while (fscanf(file, "%llu %lf %lf %lf %lf\n",
                &id, &ra, &orthoSD, &dec, &decSD) == 5) {
        objectlist_add(&dlist, Object_new(id, ra, orthoSD, dec, decSD));
    }
    return (dlist);
}

void catalog_free_objects(ObjectList* d) {
    objectlist_free(d);
}

/*
 * Read a catalog of FITS LDAC format. Must be freed with catalog_free/2.
 */
static catstruct* read_fitscat_file(char *fileName) {
    catstruct *catalog;

    if ((catalog = read_cat(fileName)) == NULL) {
        fprintf(stderr, "%s %s\n", fileName, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return (catalog);
}

/*
 * Read several catalogs (FITS LDAC).
 * Must be freed with cataloc_free/2
 */
catstruct** catalog_read_fitscat(char **inputFiles, int numInputFiles) {
    int i;
    catstruct **catalogs;
    catalogs = malloc(sizeof(catstruct*) * numInputFiles);

#pragma omp parallel for
    for (i=0; i < numInputFiles; i++) {
        catalogs[i] = read_fitscat_file(inputFiles[i]);
    }

    return (catalogs);

}

/*
 * Free catalogs created with  catalog_read_fitscat
 */
void catalog_free(catstruct **catalogs, int number) {
    free_cat(catalogs, number);
    free(catalogs);
}

/*
 * Read several catalogs (ASCII)
 */
void catalog_read_asciicat2(char **inputFiles, int numInputFiles) {
    ObjectList dlist;
    Object_T d;
    int i, j;

    for (i=0; i < numInputFiles; i++) {
        dlist = catalog_read_ascii_file(inputFiles[i]);
        for (j=0; j < dlist.size; j++) {
            d = dlist.objects[j];

            printf("id: %i %f %f %f %f\n",
                                (int) d.id, d.ra, d.dec, d.orthoSD, d.decSD);
        }
    }
    objectlist_free(&dlist);
}

/*
 * Fits table to ASCII
 */
static const int MAX_OUTPUT = 2;
void test_fits_simple_print(char **files, int numFiles) {
    catstruct **catalogs;
    catstruct *catalog;
    tabstruct *table;
    int i, j;
    int flag;

    printf("will open %i files\n", numFiles);
    catalogs = catalog_read_fitscat(files, numFiles);

    for (i=0; i<numFiles; i++) {
        catalog = catalogs[i];
        printf("Iterate catalog %i %i\n", i, catalog->ntab);

        j = 0;
        table = catalog->tab;
        flag = 0;
        while (j < catalog->ntab) {

            if (!strcmp("LDAC_OBJECTS", table->extname) ||
                    !strcmp("OBJECTS", table->extname))
            {
                show_keys(table, NULL, NULL, 0, NULL, stdout, 1, flag,
                        0, SHOW_ASCII);
            }
            table = table->nexttab;
            flag = 0;
            j++;
            if (j > MAX_OUTPUT) /* limit output to MAX_OUTPUT lines */
                break;
        }

    }
    catalog_free(catalogs, numFiles);

}
