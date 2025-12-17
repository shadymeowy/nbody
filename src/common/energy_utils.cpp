#include "energy_utils.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

#include "common/body.hpp"
#include "common/constants.hpp"

// see energy_utils.hpp for details

namespace nbodysim {

auto potentialEnergy(const std::vector<Body> &bodies) -> double {
    double total_pe = 0.0;

    // add up potential energy between each pair of bodies
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            const double dx = bodies[j].pos.v[0] - bodies[i].pos.v[0];
            const double dy = bodies[j].pos.v[1] - bodies[i].pos.v[1];
            const double dz = bodies[j].pos.v[2] - bodies[i].pos.v[2];
            const double r = std::sqrt((dx * dx) + (dy * dy) + (dz * dz));

            const double mass_prod = bodies[i].mass * bodies[j].mass;
            total_pe -= (nbodysim::constants::g_au * mass_prod) /
                        (r + nbodysim::constants::softening_au);
        }
    }

    return total_pe;
}

auto kineticEnergy(const std::vector<Body> &bodies) -> double {
    double total_ke = 0.0;

    for (const auto &b : bodies) {
        const double v2 = (b.vel.v[0] * b.vel.v[0]) +
                          (b.vel.v[1] * b.vel.v[1]) + (b.vel.v[2] * b.vel.v[2]);
        total_ke += 0.5 * b.mass * v2;
    }

    return total_ke;
}

void virialScale(std::vector<Body> &bodies) {
    // virial theorem, see the header file for explanation
    const double total_pe = potentialEnergy(bodies);
    const double total_ke = kineticEnergy(bodies);

    // 0 = 2 * KE + PE
    // so we want KE = -0.5 * PE
    const double desired_ke = -0.5 * total_pe;
    const double scale_factor = std::sqrt(desired_ke / total_ke);

    // scale velocities
    for (auto &b : bodies) {
        b.vel.v[0] *= scale_factor;
        b.vel.v[1] *= scale_factor;
        b.vel.v[2] *= scale_factor;
    }
}

void zeroMomentum(std::vector<Body> &bodies) {
    Vec3 total_momentum = {0, 0, 0};
    double total_mass = 0;

    // Calculate total momentum
    for (const auto &b : bodies) {
        total_momentum.v[0] += b.vel.v[0] * b.mass;
        total_momentum.v[1] += b.vel.v[1] * b.mass;
        total_momentum.v[2] += b.vel.v[2] * b.mass;
        total_mass += b.mass;
    }

    // Calculate velocity of the center of mass
    Vec3 vel_correction = {
        total_momentum.v[0] / total_mass,
        total_momentum.v[1] / total_mass,
        total_momentum.v[2] / total_mass,
    };

    // Subtract this drift from EVERY body (including Sun)
    for (auto &b : bodies) {
        b.vel.v[0] -= vel_correction.v[0];
        b.vel.v[1] -= vel_correction.v[1];
        b.vel.v[2] -= vel_correction.v[2];
    }
}

auto totalEnergy(const std::vector<Body> &bodies) -> double {
    return kineticEnergy(bodies) + potentialEnergy(bodies);
}

}  // namespace nbodysim