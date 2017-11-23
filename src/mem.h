/*
 * Memory allocation utilities.
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
#ifndef _MEM_H_
#define _MEM_H_

void* Mem_alloc(long nbytes);
void* Mem_calloc(long nbytes, long count);
void  Mem_free(void *ptr);
void* Mem_realloc(void *ptr, long nbytes);

/* TODO print file and lines */
/* TODO crash on alloc error */
#define ALLOC(nbytes)  Mem_alloc(nbytes)
#define CALLOC(count, nbytes) Mem_calloc(count, nbytes)
#define FREE(ptr) Mem_free(ptr); ptr = 0;
#define REALLOC(ptr, nbytes) ((ptr) = Mem_realloc(ptr, nbytes))
#define NEW(element) Mem_alloc(sizeof(element))

#endif /* _MEM_H_ */
