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

int
main(int argc, char **argv) {

    int i, j;
    long nsides = pow(2,13);
    long *cellindex, ncells;
    double radius_arcsec = 2.0;

    Field fields[2];

    Catalog_open(argv[1], &fields[0]);
    Catalog_open(argv[1], &fields[1]);

    HealpixCell **cells = Crossmatch_initCells(nsides);
    cellindex = Crossmatch_fillCells(fields, 2, cells, nsides, &ncells);

    Crossmatch_crossCells(cells, cellindex, ncells, radius_arcsec, ALGO_NEIGHBORS);


    int status = 0;
    Field f1 = fields[0];
    Set s;
    Object obj, *obj_bm;



    for (i=0; i<f1.nsets; i++) {
        s = f1.sets[i];
        for (j=0; j<s.nobjects; j++) {
            obj = s.objects[j];
            obj_bm = obj.bestMatch;

            // Every objects should match something
            if (obj_bm == NULL) {
                fprintf(stderr,"%li %f %f\n ", obj.id, obj.dec, obj.ra);
                fprintf(stderr, "\nOrphan object should not occur for set %i object %i\n", i, j);
                status = 1;
                continue;
            }

            // Every objects should not match something from their field
            if (obj.set->field == obj_bm->set->field) {
                fprintf(stderr, "\nObject should not match with himself! %i %i\n",i, j);
                status = 1;
                continue;
            }

            // match distance must be zero
            if (obj.bestMatchDistance > 0.0f) {
                fprintf(stderr, "\nBest distance should be 0 %0.50f ", obj.bestMatchDistance);
                status = 1;
                continue;
            }

            // objects matches must share the same nest id
            if (obj.pix_nest != obj_bm->pix_nest) {
                fprintf(stderr, "\n Object should share the pix id %i %i %li", i,j,obj.pix_nest);
                status = 1;
                continue;
            }

            // object id must be the same id
            if (obj.id != obj_bm->id) {
                fprintf(stderr, "\n Object should share the same id %i %i %li", i,j,obj.id);
                status = 1;
                continue;
            }

        }
    }


    Catalog_freeField(&fields[0]);
    Catalog_freeField(&fields[1]);
    Crossmatch_freeCells(cells, cellindex, ncells);

    return status;
}
