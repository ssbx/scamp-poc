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
#include <stdio.h>

void*
Mem_alloc(long nbytes, int line, char *file) {
    void *ptr;
    if (nbytes < 1)  {
        fprintf(stderr, "mem alloc fail %s %i\n", file, line);
        abort();
    }
        
    ptr = malloc(nbytes);
    if (!ptr) {
        fprintf(stderr, "mem alloc fail %s %i\n", file, line);
        abort();
    }

    return ptr;
}

void*
Mem_calloc(long nbytes, long count, int line, char *file) {
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
Mem_realloc(void *ptr, long nbytes, int line, char *file) {
    assert(ptr);
    assert(nbytes > 0);
    ptr = realloc(ptr, nbytes);
    assert(ptr);
    return ptr;
}

