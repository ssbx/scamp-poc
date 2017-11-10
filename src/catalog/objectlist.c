/*
 * Eficient list of Object strutures.
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

#include "objectlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const int SIZE_STEP = 1000;


void
Objectlist_init(ObjectList_T *l) {
    l->objects   = malloc(sizeof(Object_T) * SIZE_STEP);
    l->size     = 0;
    l->max      = SIZE_STEP;
    l->commited = false;
}


void
Objectlist_free(ObjectList_T *l) {
    free(l->objects);
}


void
Objectlist_add(ObjectList_T *l, Object_T d) {
    if (l->size == l->max) {
        l->objects = realloc(l->objects, sizeof(Object_T) * (l->max + SIZE_STEP));
        l->max    += SIZE_STEP;
    }
    l->objects[l->size] = d;
    l->size++;
}


void
Objectlist_rem(ObjectList_T *l, int i) {
	Object_T last;

    last = l->objects[l->size - 1];
    l->objects[i] = last;
    l->size--;

	/* maybe realloc */
    if (l->size * 2 < l->max &&  l->max > 2 * SIZE_STEP ) {
        l->objects   = realloc(l->objects, sizeof(Object_T) * l->size + SIZE_STEP);
        l->max      = l->size + SIZE_STEP;
    }
}


void
Objectlist_merge(ObjectList_T *l, ObjectList_T *m) {
    int newsize;

    newsize = l->size + m->size;
    l->objects = realloc(l->objects, sizeof(Object_T) * newsize + SIZE_STEP);
    l->max     = newsize + SIZE_STEP;

    memcpy(&l->objects[l->size], m->objects, m->size);

    Objectlist_free(m);
}

static int
filterByRa(const void *va, const void *vb) {
	Object_T *a, *b;

	a = (Object_T *) va;
	b = (Object_T *) vb;

	if (a->ra == b->ra)
		return 0;
	else if (a->ra > b->ra)
		return 1;
	else
		return -1;
}

void
Objectlist_commit(ObjectList_T *l) {
	if (l->commited == true)
		return;

	l->commited = true;
	qsort(l->objects, l->size, sizeof(Object_T), filterByRa);
}

int
Objectlist_length(ObjectList_T *l) {
    return l->size;
}

double
Objectlist_getMaxSd(ObjectList_T *l) {
	int i;
	double maxsd = 0;
	for (i=0; i<l->size; i++) {
		if (l->objects[i].sd > maxsd)
			maxsd = l->objects[i].sd;
	}
	return maxsd;
}

Object_T*
Objectlist_get(ObjectList_T *l, int i) {
    if (i < l->size) {
        return &l->objects[i];
    }
    return NULL;
}
