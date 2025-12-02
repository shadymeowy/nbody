#pragma once

// this file defines the simulation state
// good for saving and visualization (in the future)
// we do not generally record every state in a simulation, only at intervals
// see euler.cpp or verlet.cpp for this

#include <string>
#include <vector>

#include "common/body.hpp"

namespace nbodysim {

struct SimState {
    double time;
    std::vector<Body> bodies;
};

// save to csv
void saveSimStateToCSV(const std::vector<SimState> &states,
                       const std::string &filename);

}  // namespace nbodysim