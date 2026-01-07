#pragma once

// generate a ring scenario with random bodies in a ring shape

#include <cstdint>
#include <vector>

#include "common/body.hpp"

namespace nbodysim {

auto generateRandomRing(int num_bodies, uint32_t seed = 42,
                        double radius_au = 10.0, double width_au = 2.0,
                        double z_width = 6.0, double mass_center = 1.0,
                        double mass_astroid = 1e-4) -> std::vector<Body>;

}  // namespace nbodysim