/*
 * test_crossmatch_number.c
 *
 * Twice the same file with 5 samples, should give 5 matches.
 *
 *  Created on: 8 déc. 2017
 *      Author: serre
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../src/scamp.h"
#include "../src/mem.h"
#include "../src/catalog.h"
#include "../src/crossmatch.h"

extern void test_Catalog_open_ascii(char*, Field*);

static char t1[] = "tests/data/asciicat/t1_cat.txt";

int main(int argc, char **argv) {

    Field fields[2];
    test_Catalog_open_ascii(t1, &fields[0]);
    test_Catalog_open_ascii(t1, &fields[1]);

    long nmatches;
    long nsides = pow(2, 10);
    double radius_arcsec = 2.0;
    nmatches = Crossmatch_crossFields(fields, 2, nsides, radius_arcsec,
            ALGO_NEIGHBORS, STORE_SCHEME_AVLTREE);
    if (nmatches != 5)
        return 1;

    Catalog_freeField(&fields[0]);
    Catalog_freeField(&fields[1]);
    return 0;
}
