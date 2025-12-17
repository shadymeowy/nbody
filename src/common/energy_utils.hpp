#pragma once

// varios energy related utility functions
// virial scaling, momentum zeroing, total energy calculation

#include <vector>

#include "common/body.hpp"

namespace nbodysim {

// this function uses Virial Theorem to scale velocities
// to avoid collapsing or exploding clusters

// the theorem states that following holds for
// any closed and energy stable system with
// inverse-square law forces (like gravity):
// 2 * KE + PE = 0
// ref: https://en.wikipedia.org/wiki/Virial_theorem
void virialScale(std::vector<Body> &bodies);

// modify bodies to have zero total momentum
// good for avoiding momentum drift in simulation
void zeroMomentum(std::vector<Body> &bodies);

// calculate potential energy of the system
auto potentialEnergy(const std::vector<Body> &bodies) -> double;

// calculate kinetic energy of the system
auto kineticEnergy(const std::vector<Body> &bodies) -> double;

// calculate total energy of the system
auto totalEnergy(const std::vector<Body> &bodies) -> double;

}  // namespace nbodysim