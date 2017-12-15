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

CmLeaf*
new_leaf(CmLeaf *up, unsigned char axe, Sample *spl) {
    CmLeaf *myLeaf = ALLOC(sizeof(CmLeaf));
    myLeaf->axe = axe;
    myLeaf->sample = spl;
    myLeaf->up = up;
    myLeaf->left = NULL;
    myLeaf->right = NULL;
    myLeaf->div = spl->vector[axe];
    return myLeaf;
}

void
cmtree_insert_internal(
        CmLeaf *leaf,
        Sample *spl)
{
    double lim = spl->vector[leaf->axe];

    if (lim < leaf->div) {

        if (!leaf->left) {
            leaf->left = new_leaf(leaf, ROTATE_AXE(leaf->axe), spl);
            return;
        }

        cmtree_insert_internal(leaf->left, spl);

    } else if (lim > leaf->div) {

        if (!leaf->right) {
            leaf->right = new_leaf(leaf, ROTATE_AXE(leaf->axe), spl);
            return;
        }

        cmtree_insert_internal(leaf->right, spl);

    }

}

void
CmTree_insert(CmTree *tree, int layer, Sample *spl) {
    if (!tree->root) {
        tree->root = new_leaf(NULL, 0, spl);
        return;
    }

    cmtree_insert_internal(tree->root, spl);
}
