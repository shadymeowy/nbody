#pragma once

#include <array>
#include <iostream>

namespace nbodysim {

// 3D vector of double
struct Vec3 {
    std::array<double, 3> v{0.0, 0.0, 0.0};

    // io for debugging
    friend auto operator<<(std::ostream &os, const Vec3 &vec) -> std::ostream & {
        os << "Vec3(" << vec.v[0] << ", " << vec.v[1] << ", " << vec.v[2]
           << ")";
        return os;
    }
};

}  // namespace nbodysim