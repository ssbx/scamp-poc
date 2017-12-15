/*
 * cmtree.h
 *
 *  Created on: 15 déc. 2017
 *      Author: serre
 */

#ifndef SRC_CMTREE_H_
#define SRC_CMTREE_H_

#include "scamp.h"

typedef struct CmLeaf CmLeaf;
typedef struct CmTree CmTree;
struct CmLeaf {
    unsigned char axe;
    double   div;
    CmLeaf  *right;
    CmLeaf  *left;
    Sample  *sample;
};

struct CmTree {
    int max_radius; /* in radian */
    CmLeaf *root;
};

CmTree* CmTree_new(double max_radius);
void CmTree_insert(CmTree *root, int layer, Sample *spl);

#endif /* SRC_CMTREE_H_ */
