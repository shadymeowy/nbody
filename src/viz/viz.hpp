#pragma once

#include <vector>

#include "integrator/sim_state.hpp"

namespace nbodysim::viz {

void visualizeSimulation(const std::vector<SimState> &states,
                         float speed_factor = 1.0F, float trail_length = 1.0F,
                         unsigned trail_segments = 10, float size_min = 2.0F,
                         float size_max = 32.0F, float size_scale = 300.0F);

}  // namespace nbodysim::viz