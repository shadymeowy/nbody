#pragma once

// rather short list of constants
// yet better to have them here instead of magic numbers everywhere

namespace nbodysim::constants {

// gravitational constant in astronomical units
// value: 39.47841760435743 AU^3 M_sun^-1 year^-2
static constexpr double g_au = 39.47841760435743;

// softening factor used for avoiding singularities as possible
static constexpr double softening_au = 1e-16;

// pi constant, to lower cpp requirement to c++17
static constexpr double pi = 3.141592653589793238462643383279502884;

}  // namespace nbodysim::constants