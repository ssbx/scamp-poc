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

void
print_sample(Sample *s) {
	fprintf(stderr, "\n----------------\n");
	fprintf(stderr, "id: %li\n", s->id);
	fprintf(stderr, "lon: %lf\n", s->lon);
	fprintf(stderr, "col: %lf\n", s->col);
	fprintf(stderr, "ra: %lf\n", s->ra);
	fprintf(stderr, "dec: %lf\n", s->dec);
	fprintf(stderr, "v1: %lf\n", s->vector[0]);
	fprintf(stderr, "v2: %lf\n", s->vector[1]);
	fprintf(stderr, "v3: %lf\n", s->vector[2]);
	fprintf(stderr, "pix: %li\n", s->pix_nest);
	fprintf(stderr, "set: %p\n", s->set);
	fprintf(stderr, "bestmatch: %p\n", s->bestMatch);
	fprintf(stderr, "pix: %lf\n", s->bestMatchDistance);
	
}
int main(int argc, char **argv) {

    int i, j;
    long nsides = pow(2, 13);
    double radius_arcsec = 2.0;

    printf("hello\n"); fflush(stdout);
    PixelStore *store = PixelStore_new(nsides);
    Field fields[2];

    printf("hello\n"); fflush(stdout);
    Catalog_open(argv[1], &fields[0], store);
    Catalog_open(argv[1], &fields[1], store);
    printf("hello\n"); fflush(stdout);

    Crossmatch_crossSamples(store, radius_arcsec, 4);
    printf("hello\n"); fflush(stdout);

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
                fprintf(stderr, "\n Object should share the same id set: %i, \tmyid:"
                        " %li matchid: %li matchmatchid: %li", i, obj->id, obj_bm->id, obj_bm->bestMatch->id);
				print_sample(obj);
				print_sample(obj_bm);
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
	PixelStore_free(store);
	printf("end");

    return status;
}
