#include "vect.h"

#include "math.h"

double vect_magnitude(vect_s v)
{
    return sqrt(vect_dot(v, v));
}

vect_s vect_normalize(vect_s v)
{
    return vect_div(v, vect_magnitude(v));
}

