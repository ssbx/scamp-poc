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
    int      size;		/* current size of the list */
    int      max;		/* used internally to realloc() */
    bool	 commited;
} ObjectList_T;

/*
 * Initialize ObjectList structure
 */
void Objectlist_init(ObjectList_T *l);

/*
 * Finalize ObjectList_T. We must not add new elements after this. Will sort
 * objects by ra.
 */
void Objectlist_commit(ObjectList_T *l);

/*
 * Add a new Object to ObjectList, resize ObjectList if needed
 */
void Objectlist_add(ObjectList_T *l, Object_T d);

/*
 * Remove a Object from ObjectList, resize ObjectList if appropriate
 */
void Objectlist_rem(ObjectList_T *l, int i);

/*
 * Free ObjectList structure
 */
void Objectlist_free(ObjectList_T *l);

/*
 * Merge object Objectlist_T "m" in Objectlist_T "l", freeing "m".
 */
void Objectlist_merge(ObjectList_T *l, ObjectList_T *m);
/*
 * Return the number of elements of the list
 */
int Objectlist_length(ObjectList_T *l);

/*
 * Get the object maximum sd from the list.
 */
double Objectlist_getMaxSd(ObjectList_T *l);

/*
 * get the nth element of the list
 */
Object_T* Objectlist_get(ObjectList_T *l, int i);

#endif /* __OBJECTLIST_H__ */
