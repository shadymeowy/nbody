#pragma once
#include <cstdlib>

#include "common/vec.hpp"

// ref: good old numpy's isclose
// https://numpy.org/doc/stable/reference/generated/numpy.isclose.html
inline auto isclose(double a, double b, double atol = 1e-8, double rtol = 1e-5)
    -> bool {
    return std::abs(a - b) <= (atol + (rtol * std::abs(b)));
}

inline auto isclose3(const nbodysim::Vec3 &a, const nbodysim::Vec3 &b,
                     double atol = 1e-8, double rtol = 1e-5) -> bool {
    return isclose(a.v[0], b.v[0], atol, rtol) &&
           isclose(a.v[1], b.v[1], atol, rtol) &&
           isclose(a.v[2], b.v[2], atol, rtol);
}