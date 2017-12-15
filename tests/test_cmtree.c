/*
 * test_cmtree.c
 *
 *  Created on: 15 déc. 2017
 *      Author: serre
 */

#include <stdio.h>
#include "../src/scamp.h"
#include "../src/cmtree.h"
#include "../src/catalog.h"
#include "../src/chealpix.h"

static char file1[] = "./tests/data/fitscat/data1.fits.cat";
static char file2[] = "./tests/data/fitscat/data2.fits.cat";

void insert_in_cmtree(int layer, Field *field, CmTree *tree) {
    int i, j;
    for (i=0; i<field->nsets; i++) {
        Set set = field->sets[i];

        for (j=0; j<set.nsamples; j++) {
            Sample *spl = &set.samples[j];
            ang2vec(spl->col, spl->lon, spl->vector);
            CmTree_insert(tree, layer, spl);

        }
    }
}

#define RADIUS_ARCSEC 2.0

int
main(int argc, char **argv) {


    Field fields[2];
    Catalog_open(file1, &fields[0]);
    Catalog_open(file2, &fields[1]);

    CmTree *tree;
    tree = CmTree_new(RADIUS_ARCSEC * 3600 * TO_RAD);
    insert_in_cmtree(0, &fields[0], tree);
    insert_in_cmtree(1, &fields[0], tree);

    Catalog_freeField(&fields[0]);
    Catalog_freeField(&fields[1]);

}
