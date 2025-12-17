#pragma once

// functor class for Barnes-Hut algorithm clears
// and rebuilds octree when called

#include <vector>

#include "common/body.hpp"
#include "octree.hpp"

namespace nbodysim {

// this is the barnes-hut functor, which can be used in
// verlet and euler integrators

// after critizing ceres so many times for this
// here is the stateful functor implementation of Barnes-Hut
// (open for suggestions)

class BarnesHut {
   public:
    explicit BarnesHut(double theta_ = 0.5) : theta2{theta_ * theta_} {}

    // functor operator to calculate forces
    void operator()(std::vector<Body> &bodies, bool reset = true);

   private:
    // hold the octree nodes reference
    // it will be reused for each force calculation
    std::vector<Node> nodes;

    // theta parameter for Barnes-Hut
    double theta2;
};

}  // namespace nbodysim