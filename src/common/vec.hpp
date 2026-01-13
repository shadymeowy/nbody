#pragma once

#include <array>
#include <iostream>
#include <msgpack.hpp>

namespace nbodysim {

// 3D vector of double
struct Vec3 {
    std::array<double, 3> v{0.0, 0.0, 0.0};

    // io for debugging
    friend auto operator<<(std::ostream &os, const Vec3 &vec)
        -> std::ostream & {
        os << "Vec3(" << vec.v[0] << ", " << vec.v[1] << ", " << vec.v[2]
           << ")";
        return os;
    }

    // access operators
    auto operator[](size_t idx) -> double & {
        return v[idx];
    }
    auto operator[](size_t idx) const -> const double & {
        return v[idx];
    }

    // x, y, z accessors
    auto x() -> double & {
        return v[0];
    }
    auto y() -> double & {
        return v[1];
    }
    auto z() -> double & {
        return v[2];
    }
    auto x() const -> const double & {
        return v[0];
    }
    auto y() const -> const double & {
        return v[1];
    }
    auto z() const -> const double & {
        return v[2];
    }

    // msgpack definition
    // this is not changing memory layout nor used for simulation itself
    MSGPACK_DEFINE(v);
};

}  // namespace nbodysim