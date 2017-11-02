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

#ifndef __DATUMLIST_H__
#define __DATUMLIST_H__

#include "datum.h"

typedef struct {
    Datum *datums;
    int    size;
    int    max;
} DatumList;

void datumlist_init(DatumList *l);
void datumlist_add(DatumList *l, Datum d);
void datumlist_rem(DatumList *l, int i);
void datumlist_free(DatumList *l);
void datumlist_merge(DatumList *l, DatumList *m);

#endif // __DATUMLIST_H__
