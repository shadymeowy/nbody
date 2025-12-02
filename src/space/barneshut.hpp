#pragma once

// functor class for Barnes-Hut algorithm clears
// and rebuilds octree when called

#include <cstddef>
#include <cstdint>
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
    void operator()(std::vector<Body> &bodies, bool reset = true) {
        // we generally want to reset the octree
        // maybe reusing it for multiple steps is possible in the future?
        if (reset) {
            // clear previous octree
            nodes.clear();

            // reserve space for nodes
            nodes.reserve(bodies.size() * 2);

            // construct by inserting each body
            insertRoot(nodes, bodies, 0);
            for (size_t i = 1; i < bodies.size(); ++i) {
                insertBody(nodes, bodies, static_cast<int32_t>(i));
            }

            // finalize center of mass calculations
            finalizeNodes(nodes);
        }

        // loop over each body and calculate force using octree
        for (size_t i = 0; i < bodies.size(); ++i) {
            // reset acceleration
            bodies[i].acc = {0.0, 0.0, 0.0};
            // calculate force recursively
            calculateForceOnBody(nodes, bodies, static_cast<int32_t>(i), 0,
                                 theta2);
        }
    }

   private:
    // hold the octree nodes reference
    // it will be reused for each force calculation
    std::vector<Node> nodes;

    // theta parameter for Barnes-Hut
    double theta2;
};

}  // namespace nbodysim