#ifndef RHIZOME__QUATERNION_H
#define RHIZOME__QUATERNION_H

#include "vect.h"

typedef struct {
    double w;
    double x;
    double y;
    double z;
} quaternion_s;

extern const quaternion_s quaternion_identity;

double quaternion_magnitude(quaternion_s q);
quaternion_s quaternion_normalize(quaternion_s q);
// axis should be normalized
quaternion_s make_quaternion_rotation(vect_s axis, double angle);
// i, j, k should be normalized
quaternion_s make_quaternion_from_ijk(vect_s i, vect_s j, vect_s k);
quaternion_s make_look_quaternion(vect_s direction, vect_s up);
void print_quaternion_ijk(quaternion_s q);

static inline quaternion_s make_quaternion(
    double w,
    double x,
    double y,
    double z)
{
    quaternion_s q;
    q.w = w;
    q.x = x;
    q.y = y;
    q.z = z;
    return q;
}

static inline quaternion_s quaternion_mul(
    const quaternion_s a,
    const quaternion_s b)
{
    return make_quaternion(
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z,
        a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x);
}

static inline double quaternion_sqrmag(quaternion_s q)
{
    return q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z;
}

static inline quaternion_s quaternion_conjugate(quaternion_s q)
{
    return make_quaternion(q.w, -q.x, -q.y, -q.z);
}

static inline vect_s quaternion_rotate_vect(quaternion_s q, vect_s v)
{
    // q*v*q^-1
    quaternion_s v_quaternion = make_quaternion(0, v.x, v.y, v.z);
    quaternion_s q_inv = make_quaternion(q.w, -q.x, -q.y, -q.z);
    quaternion_s result = quaternion_mul(q, quaternion_mul(v_quaternion, q_inv));
    
    return make_vect(result.x, result.y, result.z);
}

static inline vect_s quaternion_rotate_i(quaternion_s q)
{
    return make_vect(
        1 - 2*(q.y*q.y + q.z*q.z),
        2*(q.x*q.y + q.z*q.w),
        2*(q.x*q.z - q.y*q.w));
}

static inline vect_s quaternion_rotate_j(quaternion_s q)
{
    return make_vect(
        2*(q.x*q.y - q.z*q.w),
        1 - 2*(q.x*q.x + q.z*q.z),
        2*(q.y*q.z + q.x*q.w));
}

static inline vect_s quaternion_rotate_k(quaternion_s q)
{
    return make_vect(
        2*(q.x*q.z + q.y*q.w),
        2*(q.y*q.z - q.x*q.w),
        1 - 2*(q.x*q.x + q.y*q.y));
}

static inline void quaternion_to_mat4(quaternion_s q, float *out)
{
    vect_s rot_i = quaternion_rotate_i(q);
    vect_s rot_j = quaternion_rotate_j(q);
    vect_s rot_k = quaternion_rotate_k(q);

    out[ 0] = rot_i.x; out[ 1] = rot_i.y; out[ 2] = rot_i.z; out[ 3] = 0;
    out[ 4] = rot_j.x; out[ 5] = rot_j.y; out[ 6] = rot_j.z; out[ 7] = 0;
    out[ 8] = rot_k.x; out[ 9] = rot_k.y; out[10] = rot_k.z; out[11] = 0;
    out[12] = 0;       out[13] = 0;       out[14] = 0;       out[15] = 1;
}

#endif
