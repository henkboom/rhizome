#include "quaternion.h"

#include <math.h>
#include <stdio.h>

const quaternion_s quaternion_identity = {1, 0, 0, 0};

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

// http://www.flipcode.com/documents/matrfaq.html#Q55
quaternion_s make_quaternion_from_ijk(vect_s x, vect_s y, vect_s z)
{
    quaternion_s q;

    double trace = x.x + y.y + z.z + 1;
    if(trace > 0)
    {
        double s = 0.5 / sqrt(trace);
        q.w = 0.25/s;
        q.x = (y.z - z.y) * s;
        q.y = (z.x - x.z) * s;
        q.z = (x.y - y.x) * s;
    }
    else if(x.x > y.y && x.x > z.z)
    {
        double s = sqrt(1 + x.x - y.y - z.z) * 2;
        q.x = 0.5 / s;
        q.y = (x.y + y.x) * s;
        q.w = (y.z + z.y) * s;
        q.z = (z.x + x.z) * s;
    }
    else if(y.y > z.z)
    {
        double s = sqrt(1 + y.y - x.x - z.z) * 2;
        q.w = (z.x + x.z) * s;
        q.x = (x.y + y.x) * s;
        q.y = 0.5 / s;
        q.z = (y.z + z.y) * s;
    }
    else
    {
        double s = sqrt(1 + z.z - x.x - y.y) * 2;
        q.w = (x.y + y.x) * s;
        q.x = (z.x + x.z) * s;
        q.y = (y.z + z.y) * s;
        q.z = 0.5 / s;
    }

    return q;
}

quaternion_s make_look_quaternion(vect_s direction, vect_s up)
{
    vect_s i = vect_normalize(direction);
    vect_s j = vect_normalize(vect_sub(up, vect_project(up, direction)));
    vect_s k = vect_cross(i, j);
    return make_quaternion_from_ijk(i, j, k);
}

void print_quaternion_ijk(quaternion_s q)
{
    print_vect(quaternion_rotate_i(q));
    print_vect(quaternion_rotate_j(q));
    print_vect(quaternion_rotate_k(q));
}
