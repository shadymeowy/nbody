#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "doctest.h"
#include "octree_consistency.hpp"
#include "scenario/cluster.hpp"
#include "space/octree.hpp"

namespace nb = nbodysim;

TEST_CASE("octree consistency 10") {
    // generate random bodies
    auto bodies = nb::generateRandomCluster(10);

    nb::Octree octree;
    // insert bodies by calculating forces
    octree.calculateForces(bodies);
    // get nodes for checking
    auto nodes = octree.getNodes();

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}

TEST_CASE("octree consistency 100") {
    // generate random bodies
    auto bodies = nb::generateRandomCluster(100);

    nb::Octree octree;
    // insert bodies by calculating forces
    octree.calculateForces(bodies);
    // get nodes for checking
    auto nodes = octree.getNodes();

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}

TEST_CASE("octree consistency 10000") {
    // generate random bodies
    auto bodies = nb::generateRandomCluster(10000);

    nb::Octree octree;
    // insert bodies by calculating forces
    octree.calculateForces(bodies);
    // get nodes for checking
    auto nodes = octree.getNodes();

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}