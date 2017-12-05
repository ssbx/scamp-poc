/*
 * chealpixsphere.c
 *
 *  Created on: 4 déc. 2017
 *      Author: serre
 */

#include <stdlib.h>
#include <stdio.h>

#include "chealpixsphere.h"
#include "chealpix.h"
#include "mem.h"
#include "assert.h"


static void insert_object_into_pixel(ChealpixSphere*, Object*, long);
static HealPixel* get_pixel(ChealpixSphere*,long);

void
ChealpixSphere_generate(
        ChealpixSphere *sphere,
        Field *fields,
        int nfields, long nsides) {

    if (sphere == NULL) {
        sphere = ALLOC(sizeof(ChealpixSphere));
        sphere->pixels = NULL;
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



/******************************************************************************
 * AVL related functions
 */
static HealPixel*
avl_search(HealPixel *leaf, long key) {
    if (leaf == NULL)
        return NULL;
    else if (leaf->id == key) {
        return leaf;
    } else {
        return avl_search(leaf->avlChilds[key > leaf->id], key);
    }
}

static int
avl_getHeight(HealPixel *leaf) {
    if (leaf == NULL) {
        return 0;
    } else {
        return leaf->avlHeight;
    }
}

#define Max(x,y) ((x)>(y) ? (x) : (y))
static void
avl_fixHeight(HealPixel *leaf) {
    assert(leaf != NULL);
    leaf->avlHeight = 1 + Max(avl_getHeight(leaf->avlChilds[0]), avl_getHeight(leaf->avlChilds[1]));
}

static void
avl_rotate(HealPixel *leaf, int d) {
    HealPixel *oldRoot, *newRoot, *oldMiddle;
    oldRoot = leaf;
    newRoot = oldRoot->avlChilds[d];
    oldMiddle = newRoot->avlChilds[!d];

    oldRoot->avlChilds[d] = oldMiddle;
    newRoot->avlChilds[!d] = oldRoot;
    avl_fixHeight(newRoot->avlChilds[!d]);
    avl_fixHeight(newRoot);
}

static void
avl_rebalance(HealPixel *leaf) {
    printf("avl rebalance\n");
    int i;
    if (leaf == NULL)
        return;

    for (i=0; i<2; i++) {
        if (avl_getHeight(leaf->avlChilds[i]) > avl_getHeight(leaf->avlChilds[!i]) + 1) {
            if (
                    avl_getHeight(leaf->avlChilds[i]->avlChilds[i]) <
                    avl_getHeight(leaf->avlChilds[i]->avlChilds[!i])) {
                avl_rotate(leaf, i);
            } else {
                avl_rotate(leaf->avlChilds[i], !i);
                avl_rotate(leaf, i);
            }
            return;
        }
    }

    avl_fixHeight(leaf);

}

static HealPixel*
avl_insert(HealPixel *root, HealPixel element) {

    if (root->id < 0) { // empty
        root = ALLOC(sizeof(HealPixel));
        printf("while root is %p\n", root);
        assert(root);
        *root = element;
        root->avlChilds[0] = NULL;
        root->avlChilds[1] = NULL;
        root->avlHeight = 1;
        return root;
    } else if (element.id == root->id) {
        return root;;
    } else {
        HealPixel *in = avl_insert(root->avlChilds[element.id > root->id], element);
        avl_rebalance(root);
        return in;
    }
}

int
avl_test_1() {
    HealPixel root;
    root.id = -1;

    HealPixel newElement;
    newElement.id = 1;

    printf("root before %p\n", root.id);
    avl_insert(&root, newElement);
    printf("root after %p\n"), root.id;

    return 1;
}

/**
 * AVL related functions end
 ******************************************************************************/



static void
insert_object_into_pixel(
        ChealpixSphere  *sphere,
        Object          *object,
        long             nsides)
{
    HealPixel *pixel = get_pixel(sphere, object->pix_nest);
    //printf("hello pixel %li\n", pixel->id);

}

static HealPixel*
get_pixel(ChealpixSphere *sphere, long pixelid) {
    HealPixel *pix = avl_search(sphere->pixels, pixelid);
    if (pix != NULL)
        return pix;
    HealPixel newPix;
    newPix.avlHeight = 0;
    newPix.id = pixelid;
    newPix.avlChilds[0] = NULL;
    newPix.avlChilds[1] = NULL;

    return avl_insert(sphere->pixels, newPix);

}
