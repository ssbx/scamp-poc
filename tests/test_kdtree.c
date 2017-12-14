/*
 * test_kbtree.c
 *
 *  Created on: 14 déc. 2017
 *      Author: serre
 */

#include "../src/kdtree.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define N 1000000
#define rand1() (rand() / (double)RAND_MAX)
#define rand_pt(v) { v.x[0] = rand1(); v.x[1] = rand1(); v.x[2] = rand1(); }
int main(void) {
    int i;
    kd_node wp[] = {
            { { 2, 3 } }, { { 5, 4 } }, { { 9, 6 } }, {
            { 4, 7 } }, { { 8, 1 } }, { { 7, 2 } } };
    kd_node testNode = { { 9, 2 } };
    kd_node *root, *found, *million;
    double best_dist;

    root = make_tree(wp, sizeof(wp) / sizeof(wp[1]), 0, 2);

    int visited = 0;
    found = 0;
    nearest(root, &testNode, 0, 2, &found, &best_dist, &visited);

    printf(">> WP tree\nsearching for (%g, %g)\n"
            "found (%g, %g) dist %g\nseen %d nodes\n\n", testNode.x[0],
            testNode.x[1], found->x[0], found->x[1], sqrt(best_dist), visited);


    /*
     * Here is the interesting code
     */
    million = (kd_node*) calloc(N, sizeof(kd_node));
    srand(time(0));
    for (i = 0; i < N; i++)
        rand_pt(million[i]);

    root = make_tree(million, N, 0, 3);
    rand_pt(testNode);

    visited = 0;
    found = 0;
    nearest(root, &testNode, 0, 3, &found, &best_dist, &visited);

    printf(">> Million tree\nsearching for (%g, %g, %g)\n"
            "found (%g, %g, %g) dist %g\nseen %d nodes\n", testNode.x[0],
            testNode.x[1], testNode.x[2], found->x[0], found->x[1], found->x[2],
            sqrt(best_dist), visited);

    /* search many random points in million tree to see average behavior.
     tree size vs avg nodes visited:
     10      ~  7
     100     ~ 16.5
     1000        ~ 25.5
     10000       ~ 32.8
     100000      ~ 38.3
     1000000     ~ 42.6
     10000000    ~ 46.7              */
    int sum = 0, test_runs = 100000;
    for (i = 0; i < test_runs; i++) {
        found = 0;
        visited = 0;
        rand_pt(testNode);
        nearest(root, &testNode, 0, 3, &found, &best_dist, &visited);
        sum += visited;
    }
    printf("\n>> Million tree\n"
            "visited %d nodes for %d random findings (%f per lookup)\n", sum,
            test_runs, sum / (double) test_runs);

    // free(million);

    return 0;
}
