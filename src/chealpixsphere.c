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
#include "string.h"


void ChealpixSphere_generate(ChealpixSphere *sphere, Field *fields, int nfields,
        long nsides) {

    if (sphere == NULL) {
        sphere = ALLOC(sizeof(ChealpixSphere));
        sphere->pixels = NULL;
    }

    Field field;
    Set set;
    Object *object;

    int i, j, k;
    for (i = 0; i < nfields; i++) {
        field = fields[i];

        for (j = 0; j < field.nsets; j++) {
            set = field.sets[j];

            for (k = 0; k < set.nobjects; k++) {
                object = &set.objects[k];
                object->bestMatch = NULL;
                ang2pix_nest(nsides, object->dec, object->ra,
                        &object->pix_nest);
                ang2vec(object->dec, object->ra, object->vector);
//                insert_object_into_pixel(sphere, object, nsides);

            }
        }
    }
}


/*****************************************************************************
 * AVL Tree implementation
 *
 * From the SQLite source code ext/misc/amatch.c and ext/misc/closure.c
 *
 */
typedef struct pixel_avl pixel_avl;
struct pixel_avl {
    HealPixel pixel; /* The object being stored. Key is at pixel.id */
    pixel_avl *pBefore; /* Other elements less than pixel.id */
    pixel_avl *pAfter; /* Other elements greater than pixel.id */
    pixel_avl *pUp; /* Parent element */
    short int height; /* Height of this node.  Leaf==1 */
    short int imbalance; /* Height difference between pBefore and pAfter */
};

static int pixel_cmp(long a, long b) {return a < b ? -1 : (a > b ? 1 : 0);}

/* Recompute the amatch_avl.height and amatch_avl.imbalance fields for p.
 ** Assume that the children of p have correct heights.
 */
static void amatchAvlRecomputeHeight(pixel_avl *p) {
    short int hBefore = p->pBefore ? p->pBefore->height : 0;
    short int hAfter = p->pAfter ? p->pAfter->height : 0;
    p->imbalance = hBefore - hAfter; /* -: pAfter higher.  +: pBefore higher */
    p->height = (hBefore > hAfter ? hBefore : hAfter) + 1;
}

/*
 **     P                B
 **    / \              / \
 **   B   Z    ==>     X   P
 **  / \                  / \
 ** X   Y                Y   Z
 **
 */
static pixel_avl *amatchAvlRotateBefore(pixel_avl *pP) {
    pixel_avl *pB = pP->pBefore;
    pixel_avl *pY = pB->pAfter;
    pB->pUp = pP->pUp;
    pB->pAfter = pP;
    pP->pUp = pB;
    pP->pBefore = pY;
    if (pY)
        pY->pUp = pP;
    amatchAvlRecomputeHeight(pP);
    amatchAvlRecomputeHeight(pB);
    return pB;
}

/*
 **     P                A
 **    / \              / \
 **   X   A    ==>     P   Z
 **      / \          / \
 **     Y   Z        X   Y
 **
 */
static pixel_avl *amatchAvlRotateAfter(pixel_avl *pP) {
    pixel_avl *pA = pP->pAfter;
    pixel_avl *pY = pA->pBefore;
    pA->pUp = pP->pUp;
    pA->pBefore = pP;
    pP->pUp = pA;
    pP->pAfter = pY;
    if (pY)
        pY->pUp = pP;
    amatchAvlRecomputeHeight(pP);
    amatchAvlRecomputeHeight(pA);
    return pA;
}

/*
 ** Return a pointer to the pBefore or pAfter pointer in the parent
 ** of p that points to p.  Or if p is the root node, return pp.
 */
static pixel_avl **pixelAvrFromPtr(pixel_avl *p, pixel_avl **pp) {
    pixel_avl *pUp = p->pUp;
    if (pUp == 0)
        return pp;
    if (pUp->pAfter == p)
        return &pUp->pAfter;
    return &pUp->pBefore;
}

/*
 ** Rebalance all nodes starting with p and working up to the root.
 ** Return the new root.
 */
static pixel_avl *pixelAvlBalance(pixel_avl *p) {
    pixel_avl *pTop = p;
    pixel_avl **pp;
    while (p) {
        amatchAvlRecomputeHeight(p);
        if (p->imbalance >= 2) {
            pixel_avl *pB = p->pBefore;
            if (pB->imbalance < 0)
                p->pBefore = amatchAvlRotateAfter(pB);
            pp = pixelAvrFromPtr(p, &p);
            p = *pp = amatchAvlRotateBefore(p);
        } else if (p->imbalance <= (-2)) {
            pixel_avl *pA = p->pAfter;
            if (pA->imbalance > 0)
                p->pAfter = amatchAvlRotateBefore(pA);
            pp = pixelAvrFromPtr(p, &p);
            p = *pp = amatchAvlRotateAfter(p);
        }
        pTop = p;
        p = p->pUp;
    }
    return pTop;
}

/* Search the tree rooted at p for an entry with zId.  Return a pointer
 ** to the entry or return NULL.
 */
pixel_avl *pixelAvlSearch(pixel_avl *p, const long zId) {
    int c;
    while (p && (c = pixel_cmp(zId, p->pixel.id)) != 0) {
        p = (c < 0) ? p->pBefore : p->pAfter;
    }
    return p;
}


/* Insert a new node pNew.  Return NULL on success.  If the key is not
 ** unique, then do not perform the insert but instead leave pNew unchanged
 ** and return a pointer to an existing node with the same key.
 */
pixel_avl *pixelAvlInsert(pixel_avl **ppHead, pixel_avl *pNew) {
    int c;
    pixel_avl *p = *ppHead;
    if (p == 0) {
        p = pNew;
        pNew->pUp = 0;
    } else {
        while (p) {
            c = pixel_cmp(pNew->pixel.id, p->pixel.id);
            if (c < 0) {
                if (p->pBefore) {
                    p = p->pBefore;
                } else {
                    p->pBefore = pNew;
                    pNew->pUp = p;
                    break;
                }
            } else if (c > 0) {
                if (p->pAfter) {
                    p = p->pAfter;
                } else {
                    p->pAfter = pNew;
                    pNew->pUp = p;
                    break;
                }
            } else {
                return p;
            }
        }
    }
    pNew->pBefore = 0;
    pNew->pAfter = 0;
    pNew->height = 1;
    pNew->imbalance = 0;
    *ppHead = pixelAvlBalance(p);
    return 0;
}

void pixelAvlFree(pixel_avl *pix) {
    if (!pix)
        return;

    pixelAvlFree(pix->pAfter);
    pixelAvlFree(pix->pBefore);

    FREE(pix);
}

#if 0 /* NOT USED */
/* Remove node pOld from the tree.  pOld must be an element of the tree or
** the AVL tree will become corrupt.
*/
static void amatchAvlRemove(amatch_avl **ppHead, amatch_avl *pOld){
    amatch_avl **ppParent;
    amatch_avl *pBalance = 0;
    /* assert( amatchAvlSearch(*ppHead, pOld->zKey)==pOld ); */
    ppParent = amatchAvlFromPtr(pOld, ppHead);
    if( pOld->pBefore==0 && pOld->pAfter==0 ){
        *ppParent = 0;
        pBalance = pOld->pUp;
    }else if( pOld->pBefore && pOld->pAfter ){
        amatch_avl *pX, *pY;
        pX = amatchAvlFirst(pOld->pAfter);
        *amatchAvlFromPtr(pX, 0) = pX->pAfter;
        if( pX->pAfter ) pX->pAfter->pUp = pX->pUp;
        pBalance = pX->pUp;
        pX->pAfter = pOld->pAfter;
        if( pX->pAfter ){
            pX->pAfter->pUp = pX;
        }else{
            assert( pBalance==pOld );
            pBalance = pX;
        }
        pX->pBefore = pY = pOld->pBefore;
        if( pY ) pY->pUp = pX;
        pX->pUp = pOld->pUp;
        *ppParent = pX;
    }else if( pOld->pBefore==0 ){
        *ppParent = pBalance = pOld->pAfter;
        pBalance->pUp = pOld->pUp;
    }else if( pOld->pAfter==0 ){
        *ppParent = pBalance = pOld->pBefore;
        pBalance->pUp = pOld->pUp;
    }
    *ppHead = amatchAvlBalance(pBalance);
    pOld->pUp = 0;
    pOld->pBefore = 0;
    pOld->pAfter = 0;
    /* assert( amatchAvlIntegrity(*ppHead) ); */
    /* assert( amatchAvlIntegrity2(*ppHead) ); */
}
#endif
/**
 * AVL related functions end
 ******************************************************************************/


