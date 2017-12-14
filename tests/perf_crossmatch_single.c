/*
 * Take a single catalog as argument 1 and crossmatch Nth time as argument 2.
 * perf_crossmatch_single.c
 *
 *  Created on: 7 déc. 2017
 *      Author: serre
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "../src/catalog.h"
#include "../src/crossmatch.h"
#include "../src/mem.h"


int
main(int argc, char **argv) {
    int64_t nsides = pow(2,16);
    double radius_arcsec = 2.0;

    int ncat = atoi(argv[1]);

    Field *fields = ALLOC(sizeof(Field) * ncat);

    int i, j;
    for (i=0; i<ncat; i++) {
        printf("opening catalog %i: %i\n", i, 2+i%2);
        fflush(stdout);
        Catalog_open(argv[2 + i % 2], &fields[i]);
    }

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    Crossmatch_crossFields(fields, ncat, nsides, radius_arcsec);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Crossmatch done in %lf cpu_time seconds\n", cpu_time_used);

    for (i=0; i<ncat; i++) {
        Catalog_freeField(&fields[i]);
    }

    FREE(fields);
    return 0;
}
