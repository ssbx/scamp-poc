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

#include "objectlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const int OBJECTLIST_SIZE_INIT = 200;

/*
 * Initialize ObjectList structure
 */
void objectlist_init(ObjectList *l) {
    l->objects   = malloc(sizeof(Object) * OBJECTLIST_SIZE_INIT);
    l->size     = 0;
    l->max      = OBJECTLIST_SIZE_INIT;
}

/*
 * Free ObjectList structure
 */
void objectlist_free(ObjectList *l) {
    free(l->objects);
}

/*
 * Add a new Object to ObjectList, resize ObjectList if needed
 */
void objectlist_add(ObjectList *l, Object d) {
    if (l->size == l->max) {
        l->objects    = realloc(l->objects, sizeof(Object) * l->max * 2);
        l->max      *= 2;
    }
    l->objects[l->size] = d;
    l->size++;
}

/*
 * Remove a Object from ObjectList, resize ObjectList if appropriate
 */
void objectlist_rem(ObjectList *l, int i) {
	Object last;

    last = l->objects[l->size - 1];
    l->objects[i] = last;
    l->size--;

	/* maybe realloc */
    if (l->size * 2 < l->max) {
        l->objects   = realloc(l->objects, sizeof(Object) * l->size + OBJECTLIST_SIZE_INIT);
        l->max      = l->size + OBJECTLIST_SIZE_INIT;
    }
}

/*
 * Merge object list "m" in objectlist "l", freeing "m".
 */
void objectlist_merge(ObjectList *l, ObjectList *m) {
    int newsize;

    newsize = l->size + m->size;
    l->objects = realloc(l->objects, sizeof(Object) * newsize + OBJECTLIST_SIZE_INIT);
    l->max    = newsize + OBJECTLIST_SIZE_INIT;

    memcpy(&l->objects[l->size], m->objects, m->size);

    objectlist_free(m);
}
