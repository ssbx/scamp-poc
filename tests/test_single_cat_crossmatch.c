/*
 * test_single_cat_crossmatch.c
 *
 * Crossmatch a catalog with himself. Each objects should match "themselves".
 *
 * Take a single argument with the sextractor catalog to test against.
 *
 *  Created on: 4 déc. 2017
 *      Author: serre
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "../src/catalog.h"
#include "../src/crossmatch.h"
#include "../src/pixelstore.h"

int main(int argc, char **argv) {

    int i, j;
    long nsides = pow(2, 13);
    double radius_arcsec = 2.0;

    PixelStore *store = PixelStore_new(nsides);
    Field fields[2];

    Catalog_open(argv[1], &fields[0], store);
    Catalog_open(argv[1], &fields[1], store);

    Crossmatch_crossSamples(store, radius_arcsec);

    int status = 0;
    Field f1 = fields[0];
    Set s;
    Sample *obj, *obj_bm;

    for (i = 0; i < f1.nsets; i++) {
        s = f1.sets[i];
        for (j = 0; j < s.nsamples; j++) {
            obj = s.samples[j];
            obj_bm = obj->bestMatch;

            // Every objects should match something
            if (obj_bm == NULL) {
                fprintf(stderr, "\nOrphan object should not occur for set %i "
                        "object %i\n", i, j);
                status = 1;
                continue;
            }

            // Every objects should not match something from their field
            if (obj->set->field == obj_bm->set->field) {
                fprintf(stderr, "\nObject should not match with himself!"
                        " %i %i\n", i, j);
                status = 1;
                continue;
            }

            // objects matches must share the same nest id
            if (obj->pix_nest != obj_bm->pix_nest) {
                fprintf(stderr, "\n Object should share the pix id %i %i pix:"
                        " %li match pix: %li", i, j, obj->pix_nest,
                        obj_bm->pix_nest);
                status = 1;
                continue;
            }

            // object id must have the same id
            if (obj->id != obj_bm->id) {
                fprintf(stderr, "\n Object should share the same id %i %i id:"
                        " %li match: %li", i, j, obj->id, obj_bm->id);
                status = 1;
                continue;
            }

            // match distance must be zero
            if (obj->bestMatchDistance > 0.0f) {
                fprintf(stderr, "\nBest distance should be 0.0f %0.50f ",
                        obj->bestMatchDistance);
                status = 1;
                continue;
            }
        }
    }

    Catalog_freeField(&fields[0]);
    Catalog_freeField(&fields[1]);

    return status;
}
