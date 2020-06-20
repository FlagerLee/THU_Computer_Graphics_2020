#pragma once

//some basic constant and methods
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846264338327950288
#define eps 1e-4
#define INF 0x3f3f3f3f

static const int C[10][10] = {{1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                              {1, 2, 1, 0, 0, 0, 0, 0, 0, 0},
                              {1, 3, 3, 1, 0, 0, 0, 0, 0, 0},
                              {1, 4, 6, 4, 1, 0, 0, 0, 0, 0},
                              {1, 5, 10, 10, 5, 1, 0, 0, 0, 0},
                              {1, 6, 15, 20, 15, 6, 1, 0, 0, 0},
                              {1, 7, 21, 35, 35, 21, 7, 1, 0, 0},
                              {1, 8, 28, 56, 70, 56, 28, 8, 1, 0},
                              {1, 9, 36, 84, 126, 126, 84, 36, 9, 1}};

enum Refl_t {DIFF, SPEC, REFR};
enum Algo {MonteCarlo, PPM};

double frand() {return (double)rand()/(double)RAND_MAX;} //initialize seed before using it

int gamma_trans(double x) //gamma transform
{
    if(x < 0.0) x = 0.0;
    else if(x > 1.0) x = 1.0;
    return (int)(0.5 + 255.0 * pow(x, 1.0/2.2));
}