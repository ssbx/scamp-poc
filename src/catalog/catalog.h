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

#include "objectlist.h"

/**
 * Read a catalog with a very simple ASCII format. Mainly used for testing.
 * ObjectList must be freed with catalog_free_objects/1
 */
ObjectList_T Catalog_read_ascii_file(char *file);
void Catalog_test_fits_simple_print(char **inputFies, int numFiles);

#endif /* __CATALOG_H__ */
