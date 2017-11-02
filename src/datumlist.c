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

#include "datumlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DATUMLIST_SIZE_INIT 200

/*
 * Initialize DatumList structure
 */
void datumlist_init(DatumList *l) {
    l->datums   = malloc(sizeof(Datum) * DATUMLIST_SIZE_INIT);
    l->size     = 0;
    l->max      = DATUMLIST_SIZE_INIT;
}

/*
 * Free DatumList structure
 */
void datumlist_free(DatumList *l) {
    free(l->datums);
}

/*
 * Add a new Datum to DatumList, resize DatumList if needed
 */
void datumlist_add(DatumList *l, Datum d) {
    if (l->size == l->max) {
        l->datums    = realloc(l->datums, sizeof(Datum) * l->max * 2);
        l->max      *= 2;
    }
    l->datums[l->size] = d;
    l->size++;
}

/*
 * Remove a Datum from DatumList, resize DatumList if appropriate
 */
void datumlist_rem(DatumList *l, int i) {
    Datum last;

    last = l->datums[l->size - 1];
    l->datums[i] = last;
    l->size--;

    /* maybe realoc */
    if (l->size * 2 < l->max) {
        l->datums   = realloc(l->datums, sizeof(Datum) * l->size + DATUMLIST_SIZE_INIT);
        l->max      = l->size + DATUMLIST_SIZE_INIT;
    }
}

/*
 * Merge datum list "m" in datumlist "l", freeing "m".
 */
void datumlist_merge(DatumList *l, DatumList *m) {
    int newsize;

    newsize = l->size + m->size;
    l->datums = realloc(l->datums, sizeof(Datum) * newsize + DATUMLIST_SIZE_INIT);
    l->max    = newsize + DATUMLIST_SIZE_INIT;

    memcpy(&l->datums[l->size], m->datums, m->size);

    datumlist_free(m);
}
