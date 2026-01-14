#pragma once

// this file defines the simulation state
// good for saving and visualization (in the future)
// we do not generally record every state in a simulation, only at intervals
// see euler.cpp or verlet.cpp for this

#include <msgpack.hpp>
#include <string>
#include <vector>

#include "common/body.hpp"
#include "space/octree.hpp"

namespace nbodysim {

struct SimState {
    double time;
    std::vector<Body> bodies;
    std::vector<std::vector<BCube>> octree_cubes;

    SimState() = default;
    SimState(double t, const std::vector<Body> &bodies_,
             const std::vector<std::vector<BCube>> &octree_cubes_)
        : time(t), bodies(bodies_), octree_cubes(octree_cubes_) {}
    SimState(double t, const std::vector<Body> &bodies_)
        : time(t), bodies(bodies_), octree_cubes() {}

    // msgpack definition
    // this is not changing memory layout nor used for simulation itself
    MSGPACK_DEFINE(time, bodies, octree_cubes);
};

struct SimResult {
    std::vector<SimState> states;

    // csv methods
    void saveToCSV(const std::string &filename) const;
    static auto loadFromCSV(const std::string &filename) -> SimResult;

    // msgpack methods
    void saveToMsgPack(const std::string &filename) const;
    static auto loadFromMsgPack(const std::string &filename) -> SimResult;

    // msgpack definition
    MSGPACK_DEFINE(states);
};

}  // namespace nbodysim