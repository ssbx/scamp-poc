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
#include "../src/pixelstore.h"

extern void test_Catalog_open_ascii(char*, Field*,PixelStore*);

static char t1[] = "tests/data/asciicat/t1_cat.txt";

int main(int argc, char **argv) {
    long nmatches;
    long nsides = pow(2, 10);
    double radius_arcsec = 2.0;

    PixelStore *store = PixelStore_new(nsides);
    Field fields[2];
    test_Catalog_open_ascii(t1, &fields[0], store);
    test_Catalog_open_ascii(t1, &fields[1], store);

    nmatches = Crossmatch_crossSamples(store, radius_arcsec);
    if (nmatches != 5) {
        fprintf(stderr, "have %li matches when 5 is expected\n", nmatches);
        return 1;
    }

    Catalog_freeField(&fields[0]);
    Catalog_freeField(&fields[1]);
    PixelStore_free(store);
    return 0;
}
