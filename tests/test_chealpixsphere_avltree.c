/*
 * test_chealpixsphere_avltree.c
 *
 *  Created on: 4 déc. 2017
 *      Author: serre
 */

#include <assert.h>
#include <stdio.h>
#include "../src/pixelstore.h"
#include "../src/mem.h"
/*
 * Declaration from the chealpixsphere.c file
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
pixel_avl *pixelAvlInsert(pixel_avl **store, pixel_avl *pix);
pixel_avl *pixelAvlSearch(pixel_avl *store, long key);
void pixelAvlFree(pixel_avl *store);

/* Find the first node (the one with the smallest key).
 */
static pixel_avl *pixAvlFirst(pixel_avl *p) {
    if (p)
        while (p->pBefore)
            p = p->pBefore;
    return p;
}

/*
 * Verify AVL tree integrity
 */

/* Return the node with the next larger key after p.
 */
static pixel_avl *pixelAvlNext(pixel_avl *p){
    pixel_avl *pPrev = 0;
  while( p && p->pAfter==pPrev ){
    pPrev = p;
    p = p->pUp;
  }
  if( p && pPrev==0 ){
    p = pixAvlFirst(p->pAfter);
  }
  return p;
}

static int pixelAvlIntegrity(pixel_avl *pHead) {
    pixel_avl *p;
    if( pHead==0 ) return 1;
    if( (p = pHead->pBefore)!=0 ){
        assert( p->pUp==pHead );
        assert( pixelAvlIntegrity(p) );
        assert( pixel_cmp(p->pixel.id, pHead->pixel.id)<0 );
        while( p->pAfter ) p = p->pAfter;
        assert( pixel_cmp(p->pixel.id, pHead->pixel.id)<0 );
    }
    if( (p = pHead->pAfter)!=0 ){
        assert( p->pUp==pHead );
        assert( pixelAvlIntegrity(p) );
        assert( pixel_cmp(p->pixel.id, pHead->pixel.id)>0 );
        p = pixAvlFirst(p);
        assert( pixel_cmp(p->pixel.id, pHead->pixel.id)>0 );
    }
    return 1;
}

static int pixelAvlIntegrity2(pixel_avl *pHead) {
    pixel_avl *p, *pNext;
    for(p=pixAvlFirst(pHead); p; p=pNext){
        pNext = pixelAvlNext(p);
        if( pNext==0 ) break;
        assert( pixel_cmp(p->pixel.id, pNext->pixel.id)<0 );
    }
    return 1;
}

int main(int argc, char **argv)  {
    printf("AVL test\n");
    long i;
    pixel_avl *pix, *store = NULL;

    for (i=0; i< 10000; i+=1) {
        pix = CALLOC(sizeof(pixel_avl), 1);
        pix->pixel.id = i;
        pixelAvlInsert(&store, pix);
        pixelAvlIntegrity(store);
        pixelAvlIntegrity2(store);
    }

    for (i=0; i<100100; i+=10) {
        pix = pixelAvlSearch(store, i);
        if (pix)
            assert(i == pix->pixel.id);
    }
    pixelAvlFree(store);

    return 0;
}
