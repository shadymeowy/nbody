#pragma once

// implements explicit euler integrator
// generic over force calculation function (not force function itself for now)
// simple but not very stable or accurate
// instead see verlet.hpp for better integrator with similar performance

#include <spdlog/spdlog.h>

#include <cstddef>
#include <functional>
#include <vector>

#include "common/body.hpp"
#include "common/sim_state.hpp"

namespace nbodysim {

template <typename F, typename R>
auto simulateEuler(std::vector<Body> &bodies, size_t n_steps, double dt,
                   size_t output_interval, F &&f, R &&r) -> SimResult {
    // get number of bodies and steps
    const size_t num_bodies = bodies.size();

    // initialize simulation state
    std::vector<SimState> states;
    states.reserve(n_steps);

    for (size_t block = 0; block < n_steps; block += output_interval) {
        const double current_time = static_cast<double>(block) * dt;

        // record current state
        states.emplace_back(current_time, bodies);

        // print progress
        std::invoke<R>(std::forward<R>(r), block);

        for (size_t step = 0; step < output_interval; step++) {
            // compute accelerations for each body
            std::invoke<F>(std::forward<F>(f), bodies);

            // update velocities and positions (kick)
#ifdef NBODY_USE_OPENMP
#pragma omp parallel for schedule(dynamic)
#endif
            for (size_t i = 0; i < num_bodies; i++) {
                auto &vel = bodies[i].vel;
                auto &acc = bodies[i].acc;
                auto &pos = bodies[i].pos;

                // update velocity
                vel.v[0] += acc.v[0] * dt;
                vel.v[1] += acc.v[1] * dt;
                vel.v[2] += acc.v[2] * dt;

                // update position
                pos.v[0] += vel.v[0] * dt;
                pos.v[1] += vel.v[1] * dt;
                pos.v[2] += vel.v[2] * dt;
            }
        }
    }

    // return recorded states with RVO
    return SimResult{states};
}

}  // namespace nbodysim