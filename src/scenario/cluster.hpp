#pragma once

// generate a random cluster of bodies
// useful for testing and benchmarking

#include <cstdint>
#include <vector>

#include "common/body.hpp"

namespace nbodysim {

auto generateRandomCluster(int num_bodies, uint32_t seed = 42,
                           double radius_au = 10.0)
    -> std::vector<Body>;

}  // namespace nbodysim