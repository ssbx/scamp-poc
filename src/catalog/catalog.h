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

#ifndef __CATALOG_H__
#define __CATALOG_H__

#include "fits/fitscat.h"
#include "objectlist.h"

typedef struct {
    catstruct *catalog;
    FILE *fd;
} Catalog_T;

void        Catalog_read_asciicat2(
                char **inputFiles,
                int    numInputFiles);
catstruct** Catalog_read_fitscat(
                char **inputFiles,
                int    numInputFiles);
void        Catalog_free(catstruct **cats,
                         int   number);
ObjectList_T  Catalog_read_ascii_file(char *file);

void        Catalog_test_fits_simple_print(
                char **inputFies,
                int numFiles);
#endif /* __CATALOG_H__ */
