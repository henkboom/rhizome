#include "vect.h"

#include <math.h>
#include <stdio.h>

const vect_s vect_zero = {0, 0, 0};
const vect_s vect_i = {1, 0, 0};
const vect_s vect_j = {0, 1, 0};
const vect_s vect_k = {0, 0, 1};

double vect_magnitude(vect_s v)
{
    return sqrt(vect_dot(v, v));
}

vect_s vect_normalize(vect_s v)
{
    return vect_div(v, vect_magnitude(v));
}

void print_vect(vect_s v)
{
    printf("(%.1lf, %.1lf, %.1lf)\n", v.x, v.y, v.z);
}

