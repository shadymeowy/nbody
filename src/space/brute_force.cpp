
#include "brute_force.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

#include "common/body.hpp"
#include "common/constants.hpp"
#include "common/vec.hpp"

// see brute_force.hpp for details

namespace nbodysim {

// O(n^2) brute-force force calculation
void calculateForcesBF(std::vector<Body> &bodies) {
    const size_t n_bodies = bodies.size();

    // reset all accelerations
    for (size_t i = 0; i < n_bodies; i++) {
        bodies[i].acc = {0.0, 0.0, 0.0};
    }

    // loop over each body

    // if available use parallel for
    for (int i = 0; i < n_bodies; i++) {
        // cache position
        const auto &pos_i = bodies[i].pos.v;
        // get reference to acceleration
        auto &acc_i = bodies[i].acc.v;

        // loop over other bodies to compute gravitational force
#ifdef NBODY_USE_SIMD
#pragma omp simd
#endif
        for (int j = 0; j < n_bodies; j++) {
            // cache position
            const auto &pos_j = bodies[j].pos.v;
            // get reference to acceleration
            auto &acc_j = bodies[j].acc.v;

            // find position vector respect to body i
            const double dx = pos_j[0] - pos_i[0];
            const double dy = pos_j[1] - pos_i[1];
            const double dz = pos_j[2] - pos_i[2];

            // compute distance and softened distance cubed
            const double dist2 =
                (dx * dx) + (dy * dy) + (dz * dz) + constants::softening_au;
            const double rdist = 1 / std::sqrt(dist2);
            const double rdist3 = rdist * rdist * rdist;

            // calculate potential between body i and j
            const double potential = constants::g_au * rdist3;

            // acceleration contribution from body j on body i
            const double accel_i = potential * bodies[j].mass;
            acc_i[0] += accel_i * dx;
            acc_i[1] += accel_i * dy;
            acc_i[2] += accel_i * dz;
        }
    }
}
}  // namespace nbodysim