#include "quaternion.h"

#include <math.h>

double quaternion_magnitude(quaternion_s q)
{
    return sqrt(quaternion_sqrmag(q));
}

quaternion_s quaternion_normalize(quaternion_s q)
{
    double sqrmag = quaternion_sqrmag(q);
    if(fabs(quaternion_sqrmag(q) - 1.0) > 0.000001)
    {
        double mag = sqrt(sqrmag);
        return make_quaternion(q.w/mag, q.x/mag, q.y/mag, q.z/mag);
    }
    else
    {
        return q;
    }
}

quaternion_s make_quaternion_rotation(vect_s axis, double angle)
{
    double s = sin(angle/2);
    double c = cos(angle/2);
    return make_quaternion(
        c,
        s * axis.x,
        s * axis.y,
        s * axis.z);
}
