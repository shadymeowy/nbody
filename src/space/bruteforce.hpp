#pragma once

// O(n^2) brute-force force calculation
// can be given to generic euler or verlet integrators

#include <vector>

#include "common/body.hpp"

namespace nbodysim {

void calculateForcesBF(std::vector<Body> &bodies);

// dummy functor to match octree interface
class BruteForce {
   public:
    // functor operator to calculate forces
    void operator()(std::vector<Body> &bodies) const {
        calculateForcesBF(bodies);
    }
};

}  // namespace nbodysim