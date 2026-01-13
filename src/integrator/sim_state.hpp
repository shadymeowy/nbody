#pragma once

// this file defines the simulation state
// good for saving and visualization (in the future)
// we do not generally record every state in a simulation, only at intervals
// see euler.cpp or verlet.cpp for this

#include <msgpack.hpp>
#include <string>
#include <vector>

#include "common/body.hpp"

namespace nbodysim {

struct SimState {
    double time;
    std::vector<Body> bodies;

    SimState() = default;
    SimState(double t, const std::vector<Body> &bodies_)
        : time(t), bodies(bodies_) {}

    // msgpack definition
    // this is not changing memory layout nor used for simulation itself
    MSGPACK_DEFINE(time, bodies);
};

struct SimResult {
    std::vector<SimState> states;

    MSGPACK_DEFINE(states);
};

// save to csv
void saveSimResultToCSV(const SimResult &states, const std::string &filename);

// save to msgpack binary
void saveSimResultToMsgPack(const SimResult &states,
                            const std::string &filename);

// load from msgpack binary
auto loadSimResultFromMsgPack(const std::string &filename) -> SimResult;

}  // namespace nbodysim