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

    // build octree
    std::vector<nb::Node> nodes{};
    nb::insertRoot(nodes, bodies, 0);
    for (size_t i = 1; i < bodies.size(); ++i) {
        nb::insertBody(nodes, bodies, static_cast<int32_t>(i), 0);
    }
    nb::finalizeNodes(nodes);

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}

TEST_CASE("octree consistency 100") {
    // generate random bodies
    auto bodies = nb::generateRandomCluster(100);

    // build octree
    std::vector<nb::Node> nodes{};
    nb::insertRoot(nodes, bodies, 0);
    for (size_t i = 1; i < bodies.size(); ++i) {
        nb::insertBody(nodes, bodies, static_cast<int32_t>(i), 0);
    }
    nb::finalizeNodes(nodes);

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}

TEST_CASE("octree consistency 10000") {
    // generate random bodies
    auto bodies = nb::generateRandomCluster(10000);

    // build octree
    std::vector<nb::Node> nodes{};
    nb::insertRoot(nodes, bodies, 0);
    for (size_t i = 1; i < bodies.size(); ++i) {
        nb::insertBody(nodes, bodies, static_cast<int32_t>(i), 0);
    }
    nb::finalizeNodes(nodes);

    // check octree integrity
    checkOctreeIntegrity(nodes, bodies);
}