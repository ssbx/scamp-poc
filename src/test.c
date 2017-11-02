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

#include "catalog.h"
#include <stdio.h>
#include <string.h>

void test_ascii_simple_cross(char **files, int numFiles) {
    catalog_read_asciicat(files, numFiles);
}

#define MAX_OUTPUT 2
/*
 * Fits table to ascii
 */
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
        printf("iterate catalog %i\n", i);

        j = 0;
        table = catalog->tab;
        flag = 0;
        while (j < catalog->ntab) {

            if (!strcmp("LDAC_OBJECTS", table->extname) ||
                !strcmp("OBJECTS", table->extname)) 
            {
                show_keys(table, NULL, NULL, 0, NULL, stdout, 1, flag, 0, SHOW_ASCII);
            }
            table = table->nexttab;
            flag = 0;
            j++;
            if (j > MAX_OUTPUT) // limit output to MAX_OUTPUT lines
                break;
        }

    }

}
