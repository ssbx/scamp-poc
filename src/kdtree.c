/*
 * kbtree.c
 *
 *  Created on: 14 déc. 2017
 *      Author: serre
 */
#include "kdtree.h"


#include <stdlib.h>
#include <string.h>
#include <time.h>

inline double dist(kd_node *a, kd_node *b, int dim) {
    double t, d = 0;
    while (dim--) {
        t = a->x[dim] - b->x[dim];
        d += t * t;
    }
    return d;
}
inline void swap(kd_node *x, kd_node *y) {
    double tmp[MAX_DIM];
    memcpy(tmp, x->x, sizeof(tmp));
    memcpy(x->x, y->x, sizeof(tmp));
    memcpy(y->x, tmp, sizeof(tmp));
}

/* see quickselect method */
 kd_node*
find_median(kd_node *start, kd_node *end, int idx) {
    if (end <= start)
        return NULL;
    if (end == start + 1)
        return start;

    kd_node *p, *store, *md = start + (end - start) / 2;
    double pivot;
    while (1) {
        pivot = md->x[idx];

        swap(md, end - 1);
        for (store = p = start; p < end; p++) {
            if (p->x[idx] < pivot) {
                if (p != store)
                    swap(p, store);
                store++;
            }
        }
        swap(store, end - 1);

        /* median has duplicate values */
        if (store->x[idx] == md->x[idx])
            return md;

        if (store > md)
            end = store;
        else
            start = store;
    }
}

 kd_node*
make_tree( kd_node *t, int len, int i, int dim) {
     kd_node *n;

    if (!len)
        return 0;

    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->left = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}


void nearest(kd_node *root, kd_node *nd, int i, int dim,
        kd_node **best, double *best_dist, int *visited) {
    double d, dx, dx2;

    if (!root)
        return;
    d = dist(root, nd, dim);
    dx = root->x[i] - nd->x[i];
    dx2 = dx * dx;

    (*visited)++;

    if (!*best || d < *best_dist) {
        *best_dist = d;
        *best = root;
    }

    /* if chance of exact match is high */
    if (!*best_dist)
        return;

    if (++i >= dim)
        i = 0;

    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist, visited);
    if (dx2 >= *best_dist)
        return;
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist, visited);
}
