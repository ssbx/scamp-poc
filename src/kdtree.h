/*
 * kbtree.h
 *
 *  Created on: 14 déc. 2017
 *      Author: serre
 */

#ifndef SRC_KDTREE_H_
#define SRC_KDTREE_H_

#define MAX_DIM 3

typedef struct kd_node kd_node;
struct kd_node {
    double x[MAX_DIM];
    kd_node *left, *right;
    void *data;
};

double dist(kd_node*, kd_node*, int);
void swap(kd_node*, kd_node*);
kd_node* find_median(kd_node*, kd_node*, int);
kd_node* make_tree(kd_node*, int, int, int);
void nearest(kd_node*, kd_node*, int, int, kd_node**, double*,int*);

#endif /* SRC_KDTREE_H_ */
