/*
 * test_chealpix_neighbours_nest.c
 *
 *  Created on: 28 nov. 2017
 *      Author: serre
 */

#include <stdio.h>
#include "../src/chealpix.h"
#include <stdint.h>

int main(int argc, char **argv) {
    long i;
    int j;
    int64_t nsides = 16;
    int64_t neigh[8];

    for (i=0; i<nside2npix(nsides); i++) {
        neighbours_nest64(nsides, i, neigh);
        printf("For %li\n", i);
        for (j=0; j<8; j++) {
            printf("Have neighbor: %li\n", neigh[j]);
        }
    }

    return 0;
}
