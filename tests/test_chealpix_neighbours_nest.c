/*
 * test_chealpix_neighbours_nest.c
 *
 *  Created on: 28 nov. 2017
 *      Author: serre
 */

#include <stdio.h>
#include "../src/chealpix.h"

int main(int argc, char **argv) {
    long i;
    int j;
    long nsides = 16;
    long neigh[8];

    for (i=1; i<nside2npix(nsides); i++) {
        neighbours_nest(nsides, i, neigh);
        printf("For %li\n", i);
        for (j=0; j<8; j++) {
            printf("Have neighbor: %li\n", neigh[j]);
        }
    }

    return 0;
}
