#pragma once

// O(n^2) brute-force force calculation
// can be given to generic euler or verlet integrators

#include <vector>

#include "common/body.hpp"

namespace nbodysim {

// calculate gravitational forces on bodies using brute-force O(n^2) method
// ie compute pairwise forces between all bodies
// modifies bodies' accelerations inplace
void calculateForcesBF(std::vector<Body> &bodies);

}  // namespace nbodysim