/*
 * Logger utils.
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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void*
Mem_alloc(long nbytes) {
    void *ptr;
    assert(nbytes > 0);
    ptr = malloc(nbytes);
    assert(ptr);
    return ptr;
}

void*
Mem_calloc(long nbytes, long count) {
    void *ptr;
    assert(nbytes > 0);
    assert(count > 0);
    ptr = calloc(count, nbytes);
    assert(ptr != NULL);
    return ptr;
}

void
Mem_free(void *ptr) {
    if(ptr)
        free(ptr);
}

void*
Mem_realloc(void *ptr, long nbytes) {
    assert(ptr);
    assert(nbytes > 0);
    ptr = realloc(ptr, nbytes);
    assert(ptr);
    return ptr;
}

