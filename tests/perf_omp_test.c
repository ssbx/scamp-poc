#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/scamp.h"
#include "../src/pixelstore.h"
#define MAX_THREADS 8

double step;

typedef struct pixel_avl pixel_avl;
struct pixel_avl {
    HealPixel pixel; /* The samples being stored. Key is at pixel.id */
    pixel_avl *pBefore; /* Other elements less than pixel.id */
    pixel_avl *pAfter; /* Other elements greater than pixel.id */
    pixel_avl *pUp; /* Parent element */
    short int height; /* Height of this node.  Leaf==1 */
    short int imbalance; /* Height difference between pBefore and pAfter */
};

#define N 1000000000

int main(int argc, const char *argv[]) {

//    int i;
//    double x;
//    double pi, sum = 0.0;
//
//    step = 1.0 / (double) steps;
//
//    sum = 0.0;
//    start = omp_get_wtime();
//
//#pragma omp parallel for reduction(+:sum) private(x)
//    for (i = 0; i < steps; i++) {
//        x = (i + 0.5) * step;
//        sum += 4.0 / (1.0 + x * x);
//    }
//
//    // Out of the parallel region, finialize computation
//    pi = step * sum;
//    delta = omp_get_wtime() - start;
//    printf("PI = %.16g computed in %.4g seconds\n", pi, delta);
//
    printf("sizeof pixel_avl is %lui\n", sizeof(pixel_avl));

    printf("sizeof HealPixel is %lui\n", sizeof(HealPixel) - 8 * sizeof(void*));
    printf("sizeof neighbors is %lui\n", sizeof(void*) * 8);
    printf("sizeof sample is %li\n", sizeof(Sample));
    return 0;
}

















