#pragma once

// a body in the simulation
// includes position, velocity, acceleration, and mass
// does not do anything fancy

#include <iostream>

#include "common/vec.hpp"

namespace nbodysim {

// a body in the simulation
struct Body {
    Vec3 pos{0.0, 0.0, 0.0};
    Vec3 vel{0.0, 0.0, 0.0};
    Vec3 acc{0.0, 0.0, 0.0};
    double mass{0.0};

    // io for debugging
    friend auto operator<<(std::ostream &os, const Body &body) -> std::ostream & {
        os << "Body(pos=" << body.pos << ", vel=" << body.vel
           << ", acc=" << body.acc << ", mass=" << body.mass << ")";
        return os;
    }
};

}  // namespace nbodysim