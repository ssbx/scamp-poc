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

#include "../src/catalog.h"
#include "../src/crossmatch.h"
#include "../src/mem.h"


static char testfile[] = "tests/data/fitscat/data1.fits.cat";

int
main(int argc, char **argv) {
    int64_t nsides = pow(2,31);
    double radius_arcsec = 2.0;
    int n = 10;
    printf("sizeof int is %li\n", sizeof(int));
    printf("sizeof int64 is %li\n", sizeof(int64_t));
    printf("sizeof long is %li\n", sizeof(long));


    Field *fields = ALLOC(sizeof(Field) * n);

    int i;
    for (i=0; i<n; i++) {
        printf("opening catalog %i\n", i);
        Catalog_open(testfile, &fields[i]);
    }

    Crossmatch_crossFields(fields, n, nsides, radius_arcsec,
            ALGO_NEIGHBORS, STORE_SCHEME_AVLTREE);

    for (i=0; i<n; i++) {
        Catalog_freeField(&fields[i]);
    }

    return 0;
}
