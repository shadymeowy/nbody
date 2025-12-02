#pragma once

#include <array>
#include "common/body.hpp"

namespace nbodysim::solar {

// Source: NASA JPL Horizons, epoch J2000
// Positions in AU, velocities in AU/day, masses in solar masses
// ref: https://ssd.jpl.nasa.gov/horizons/app.html#
static constexpr std::array<Body, 10> bodies_j2000 = {
    // 0. Sun
    Body{.pos = {0.00750, 0.00465, 0.00016},
         .vel = {-0.00269, 0.00298, 0.00004},
         .mass = 1.0},

    // 1. Earth
    Body{
        .pos = {-0.16546, 0.96939, -0.00013},
        .vel = {-6.29906, -1.06646, 0.00028},
        .mass = 3.0035e-6,
    },

    // 2. Mercury
    Body{.pos = {-0.12876, -0.42817, -0.02982},
         .vel = {7.73359, -3.85694, -0.85404},
         .mass = 1.6601e-7},

    // 3. Venus
    Body{.pos = {-0.71769, -0.04659, 0.04397},
         .vel = {0.53697, -7.36924, -0.41300},
         .mass = 2.4478e-6},

    // 4. Mars
    Body{.pos = {-1.42433, -0.84078, -0.02422},
         .vel = {3.42151, -4.50567, -0.13401},
         .mass = 3.2272e-7},

    // 5. Jupiter
    Body{.pos = {4.21855, -2.83515, -0.11189},
         .vel = {1.51737, 2.37397, -0.05929},
         .mass = 9.5479e-4},

    // 6. Saturn
    Body{.pos = {6.59238, 6.78457, -0.36653},
         .vel = {-1.52097, 1.39860, 0.07357},
         .mass = 2.8588e-4},

    // 7. Uranus
    Body{.pos = {14.17046, -12.68217, -0.23727},
         .vel = {0.96317, 1.07722, -0.00244},
         .mass = 4.3662e-5},

    // 8. Neptune
    Body{.pos = {20.80376, -21.49392, -0.72264},
         .vel = {0.81987, 0.81745, -0.02981},
         .mass = 5.1514e-5},

    // 9. Pluto
    Body{.pos = {-11.16139, -27.81744, 13.06584},
         .vel = {1.07844, -0.10651, -0.32049},
         .mass = 7.396e-9}};

// Simplified solar system bodies with circular orbits
static constexpr std::array<Body, 10> bodies_simple = {
    // Sun
    Body{.pos = {0.0, 0.0, 0.0}, .vel = {0.0, 0.0, 0.0}, .mass = 1.0},

    // Earth
    Body{.pos = {1.0, 0.0, 0.0}, .vel = {0.0, 6.283, 0.0}, .mass = 3.00e-6},

    // Mercury
    Body{.pos = {0.387, 0.0, 0.0}, .vel = {0.0, 9.993, 0.0}, .mass = 1.66e-7},

    // Venus
    Body{.pos = {0.723, 0.0, 0.0}, .vel = {0.0, 7.380, 0.0}, .mass = 2.45e-6},

    // Mars
    Body{.pos = {1.524, 0.0, 0.0}, .vel = {0.0, 5.082, 0.0}, .mass = 3.23e-7},

    // Jupiter
    Body{.pos = {5.203, 0.0, 0.0}, .vel = {0.0, 2.762, 0.0}, .mass = 9.54e-4},

    // Saturn
    Body{.pos = {9.572, 0.0, 0.0}, .vel = {0.0, 2.045, 0.0}, .mass = 2.86e-4},

    // Uranus
    Body{.pos = {19.16, 0.0, 0.0}, .vel = {0.0, 1.434, 0.0}, .mass = 4.36e-5},

    // Neptune
    Body{.pos = {30.18, 0.0, 0.0}, .vel = {0.0, 1.139, 0.0}, .mass = 5.12e-5},

    // Pluto (Dwarf)
    Body{.pos = {39.48, 0.0, 0.0}, .vel = {0.0, 0.991, 0.0}, .mass = 6.54e-9},
};

}  // namespace nbodysim::solar