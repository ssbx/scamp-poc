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

#ifndef __CROSSMATCH_H__
#define __CROSSMATCH_H__

#include "scamp.h"
#include "pixelstore.h"

extern long
Crossmatch_crossSamples(PixelStore *store, double radius_arcsec, int nthreads);

#endif /* __CROSSMATCH_H__ */
