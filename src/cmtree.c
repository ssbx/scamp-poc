/*
 * cmtree.c insert and immediately recognize objects from different fields
 * to match each others.
 *
 *  Created on: 15 déc. 2017
 *      Author: serre
 */


#include "cmtree.h"
#include "../src/mem.h"
#include <stdlib.h>
#include <stdio.h>

#define ROTATE_AXE(axe) (axe > 1) ? 0: axe + 1

CmTree*
CmTree_new(double max_radius) {
    CmTree *tree = ALLOC(sizeof(struct CmTree));
    tree->max_radius = max_radius;
    tree->root = NULL;
    return tree;
}

void
cmtree_insert_internal(
        CmLeaf **leaf,
        Sample *spl,
        unsigned char parent_axe,
        double max_radius)
{
    CmLeaf *myLeaf = *leaf;
    if (myLeaf == NULL) {
        myLeaf = ALLOC(sizeof(CmLeaf));
        myLeaf->axe = ROTATE_AXE(parent_axe);
        myLeaf->sample = spl;
        myLeaf->left = NULL;
        myLeaf->right = NULL;
        myLeaf->div = spl->vector[myLeaf->axe];
        (*leaf) = myLeaf;
        return;
    }
    unsigned char current_axe = myLeaf->axe;
    double spl_div = spl->vector[current_axe];
    if (spl_div < myLeaf->div) {
        cmtree_insert_internal(&myLeaf->left, spl, current_axe);
    } else if (spl_div > myLeaf->div) {
        cmtree_insert_internal(&myLeaf->right, spl, current_axe);
    } else {

    }

}

void
CmTree_insert(CmTree *tree, int layer, Sample *spl) {
    cmtree_insert_internal(&tree->root, spl, 0, tree->max_radius);
}
