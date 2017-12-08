/*
 * test_angdist.c
 *
 * Test the angdist function taken from the C++ healpix lib.
 *
 *  Created on: 8 déc. 2017
 *      Author: serre
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/chealpix.h"
#include "../src/scamp.h"

void debug_vec(double *v) {
    printf("0 is %lf\n", v[0]);
    printf("1 is %lf\n", v[1]);
    printf("2 is %lf\n", v[2]);
}

static long eps = 0.0001;
int
main(int argc, char **argv) {
    double v1[3], v2[3];
    double dist;

    ang2vec(0.0, 0.0, v1);
    ang2vec(0.0, 0.0, v2);
    dist = angdist(v1, v2);
    // should be near 0
    printf("dist 1 is %0.50lf\n", dist);
    if ((abs(0 - dist)) > eps) {
        return 1;
    }



    ang2vec(SC_PI / 2, 0.0, v1);
    ang2vec(-SC_PI / 2, 0.0, v2);
    dist = angdist(v1, v2);
    // should be near SC_PI
    printf("dist 2 is %0.50lf\n", dist);
    if (abs(SC_PI - dist) > eps) {
        return 1;
    }

    ang2vec(SC_PI / 2, 0.0, v1);
    ang2vec(0.0, 0.0, v2);
    dist = angdist(v1, v2);
    // should be near SC_PI / 2
    printf("dist 3 is %0.50lf\n", dist);
    if (abs((SC_PI/2) - dist) > eps) {
        return 1;
    }

    ang2vec(0.0, 2 * SC_PI, v1);
    ang2vec(0.0, 0.0, v2);
    dist = angdist(v1, v2);
    // should be near PI
    printf("dist 4 is %0.50lf\n", dist);
    if (abs(SC_PI - dist) > eps) {
        return 1;
    }

    return 0;
}
