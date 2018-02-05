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

void* Mem_alloc(long nbytes, int line, char *file);
void* Mem_calloc(long nbytes, long count, int line, char *file);
void  Mem_free(void *ptr, int line, char *file);
void* Mem_realloc(void *ptr, long nbytes, int line, char *file);

/* TODO print file and lines */
/* TODO crash on alloc error */
#define ALLOC(nbytes)  Mem_alloc(nbytes, __LINE__, __FILE__)
#define CALLOC(count, nbytes) Mem_calloc(count, nbytes, __LINE__, __FILE__)
#define FREE(ptr) {Mem_free(ptr, __LINE__, __FILE__); ptr = (void*) 0;}
#define REALLOC(ptr, nbytes) Mem_realloc(ptr, nbytes, __LINE__, __FILE__)
#define NEW(element) Mem_alloc(sizeof(element))

#endif /* _MEM_H_ */
