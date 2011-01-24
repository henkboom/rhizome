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
        exit(0);
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
    double sin_angle = sin(angle);
    double cos_angle = cos(angle);
    return make_quaternion(
        cos_angle,
        sin_angle * axis.x,
        sin_angle * axis.y,
        sin_angle * axis.z);
}

