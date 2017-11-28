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

#include "catalog.h"

typedef struct Matches {

} Matches;

//extern void Crossmatch_crossfields(Field *fields, int nfields, ObjectZone **zones);
extern void Crossmatch_crosszone(HealpixCell **zones, long *zoneindex, long nzoneindex);


#endif /* __CROSSMATCH_H__ */
