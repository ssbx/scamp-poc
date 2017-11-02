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

void catalog_read_asciicat(char **inputFiles, int numInputFiles);
catstruct** catalog_read_fitscat(char **inputFiles, int numInputFiles);

#endif // __CATALOG_H__
