//
// Created by Doruk Gürkan on 12.11.2025.
//

#ifndef NBODY_BODY_H
#define NBODY_BODY_H
#include <cmath>
#include <iostream>

constexpr double G = 6.674e-11;

struct Vec3 {
    double v[3];
};

struct Body {
    uint32_t id;
    double mass;
    Vec3 pos;
    Vec3 vel;
    Vec3 force;
    double radius;
};

inline double square(double d);

inline Vec3 minus(const Vec3 &v1, const Vec3 &v2);

inline Vec3 dot(const Vec3 &v1, const Vec3 &v2);

inline Vec3 scale(const Vec3 &v, double alpha);

inline double length_square(const Vec3 &v);

inline double length(const Vec3 &v);

Vec3 calculate_attraction(const Body &b1, const Body &b2);

void BarnesHutApproximation(std::vector<Body> &bodies, double dt, double theta,
                            uint32_t steps);

#endif  // NBODY_BODY_H
