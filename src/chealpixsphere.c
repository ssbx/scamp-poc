/*
 * chealpixsphere.c
 *
 *  Created on: 4 déc. 2017
 *      Author: serre
 */

#include <stdlib.h>

#include "chealpixsphere.h"
#include "chealpix.h"
#include "mem.h"


static void insert_object_into_pixel(ChealpixSphere*, Object*, long);
static HealPixel* get_pixel(ChealpixSphere*,long);

void
ChealpixSphere_generate(
        ChealpixSphere *sphere,
        Field *fields,
        int nfields, long nsides) {

    if (sphere == NULL) {
        sphere = ALLOC(sizeof(ChealpixSphere));
        sphere->pixels = ALLOC(sizeof(HealPixel) * 1000);
        sphere->size = 1000;
        sphere->npixels = 0;
    }

    Field   field;
    Set     set;
    Object  *object;

    int i, j, k;
    for (i=0; i<nfields; i++) {
        field = fields[i];

        for (j=0; j<field.nsets; j++) {
            set = field.sets[j];

            for (k=0; k<set.nobjects; k++) {
                object = &set.objects[k];
                object->bestMatch = NULL;
                ang2pix_nest(nsides, object->dec, object->ra, &object->pix_nest);
                ang2vec(object->dec, object->ra, object->vector);
                insert_object_into_pixel(sphere, object, nsides);

            }
        }
    }
}

static void
insert_object_into_pixel(
        ChealpixSphere  *sphere,
        Object          *object,
        long             nsides)
{
    HealPixel *pixel = get_pixel(sphere, object->pix_nest);

}

static HealPixel*
get_pixel(ChealpixSphere *sphere, long pixelid) {
    int i;

    for (i=0; i<sphere->npixels; i++) {
        if (sphere->pixels[i].id == pixelid) {
            return &sphere->pixels[i];
        }
    }

    /*
     *  not found, create it:
     */

    /* need realloc */
    if (sphere->npixels == sphere->size) {
        sphere->pixels = REALLOC(sphere->pixels, sizeof(HealPixel) * sphere->size * 2);
        sphere->size *= 2;
    }

    HealPixel pix;
    pix.nobjects = 0;
    pix.objects = ALLOC(sizeof(Object*) * 1000);
    pix.size = 1000;
    sphere->pixels[sphere->npixels] = pix;
    sphere->npixels ++;
    return &sphere->pixels[sphere->npixels - 1];

}
