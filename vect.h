#ifndef RHIZOME__VECT_H
#define RHIZOME__VECT_H

typedef struct {
    double x;
    double y;
    double z;
} vect_s;

extern const vect_s vect_zero;
extern const vect_s vect_i;
extern const vect_s vect_j;
extern const vect_s vect_k;

static inline vect_s make_vect(double x, double y, double z)
{
    vect_s v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static inline vect_s vect_add(vect_s a, vect_s b)
{
    return make_vect(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline vect_s vect_sub(vect_s a, vect_s b)
{
    return make_vect(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline vect_s vect_neg(vect_s v)
{
    return make_vect(-v.x, -v.y, -v.z);
}

static inline vect_s vect_mul(vect_s v, double s)
{
    return make_vect(v.x * s, v.y * s, v.z * s);
}

static inline vect_s vect_div(vect_s v, double s)
{
    return make_vect(v.x / s, v.y / s, v.z / s);
}

static inline double vect_dot(vect_s a, vect_s b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline double vect_sqrmag(vect_s v)
{
    return vect_dot(v, v);
}

static inline int vect_equal(vect_s a, vect_s b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

static inline vect_s vect_cross(vect_s a, vect_s b)
{
    return make_vect(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x);
}

static inline vect_s vect_project(vect_s a, vect_s b)
{
    return vect_mul(b, vect_dot(a, b) / vect_sqrmag(b));
}

double vect_magnitude(vect_s v);

vect_s vect_normalize(vect_s v);

#endif
