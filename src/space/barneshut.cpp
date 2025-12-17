#include "barneshut.hpp"

#include <cstddef>
#include <cstdint>

// see bruteforce.hpp for details

namespace nbodysim {

void BarnesHut::operator()(std::vector<Body> &bodies, bool reset) {
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
        calculateForceOnBody(nodes, bodies, static_cast<int32_t>(i), 0, theta2);
    }
}

}  // namespace nbodysim