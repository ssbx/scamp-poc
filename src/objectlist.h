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

#ifndef __OBJECTLIST_H__
#define __OBJECTLIST_H__

#include "object.h"

typedef struct {
    Object_T *objects;
    int      size;
    int      max;
} ObjectList;

void objectlist_init(ObjectList *l);
void objectlist_add(ObjectList *l, Object_T d);
void objectlist_rem(ObjectList *l, int i);
void objectlist_free(ObjectList *l);
void objectlist_merge(ObjectList *l, ObjectList *m);

#endif /* __OBJECTLIST_H__ */
