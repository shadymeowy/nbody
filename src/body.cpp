//
// Created by Doruk Gürkan on 15.11.2025.
//
#include "body.h"

#include <algorithm>

inline double square(const double d) { return d * d; }

inline Vec3 minus(const Vec3 &v1, const Vec3 &v2) {
    Vec3 v = {};
    for (size_t i = 0; i < 3; i++) {
        v.v[i] = v1.v[i] - v2.v[i];
    }
    return v;
}

inline Vec3 dot(const Vec3 &v1, const Vec3 &v2) {
    auto v = Vec3{};
    for (size_t i = 0; i < 3; i++) {
        v.v[i] = v1.v[i] * v2.v[i];
    }
    return v;
}

inline Vec3 scale(const Vec3 &v, double alpha) {
    return Vec3{
        v.v[0] * alpha,
        v.v[1] * alpha,
        v.v[2] * alpha,
    };
}

inline double length_square(const Vec3 &v) {
    auto v_squared = dot(v, v);
    double sum = 0l;
    for (const double i : v_squared.v) {
        sum += i;
    }
    return sum;
}

inline double length(const Vec3 &v) { return std::sqrt(length_square(v)); }

inline Vec3 normalize(const Vec3 v) noexcept {
    auto l = length(v);
    auto result = v;
    for (auto &d : result.v) {
        d /= l;
    }
    return result;
}

Vec3 calculate_attraction(const Body &b1, const Body &b2) {
    const Vec3 v = minus(b1.pos, b2.pos);  // distance between the two bodies
    const double r_squared = length_square(v);
    const double force_net = -G * b2.mass / r_squared;
    const Vec3 force = scale(normalize(v), force_net);
    return force;
}
