#pragma once

// implements symplectic "velocity verlet integrator"
// generic over force calculation function (not force function itself, yet)

// verlet integration is a symplectic integrator commonly used in nbody and
// similar simulations. as in the explicit euler method, it only requires one
// force calculation per step but it is much more stable and accurate (order 2)
// ref: https://en.wikipedia.org/wiki/Verlet_integration

// we default to it unless specified otherwise
// euler is still available for testing and comparison (euler.hpp)
// but even for simple SOLAR scenario, it shows significant energy drift

#include <cstddef>
#include <iostream>
#include <vector>

#include "common/body.hpp"
#include "integrator/simstate.hpp"

namespace nbodysim {

template <typename F>
auto simulateVerlet(std::vector<Body> &bodies, size_t n_steps, double dt,
                    size_t output_interval, F &f_force)
    -> std::vector<SimState> {
    // get number of bodies and steps
    const size_t num_bodies = bodies.size();

    // half dt and dt for convenience
    const double half_dt = dt / 2.0;

    // initialize simulation state
    std::vector<SimState> states;
    states.reserve(n_steps);

    // initial force calculation
    f_force(bodies);

    for (size_t block = 0; block < n_steps; block += output_interval) {
        const double current_time = static_cast<double>(block) * dt;

        // record current state
        states.emplace_back(current_time, bodies);

        // print progress
        // TODO: use proper logging system
        std::cout << "\rSimulated " << (block * 100) / n_steps << "% of steps."
                  << std::flush;

        // perform output_interval steps
        // hope is that compiler will optimize this loop well
        for (size_t step = 0; step < output_interval; step++) {
            // half velocity step and full position step (kick-drift)
            for (size_t i = 0; i < num_bodies; i++) {
                auto &vel = bodies[i].vel;
                auto &pos = bodies[i].pos;
                auto &acc = bodies[i].acc;

                // half velocity step
                vel.v[0] += acc.v[0] * half_dt;
                vel.v[1] += acc.v[1] * half_dt;
                vel.v[2] += acc.v[2] * half_dt;

                // full position step
                pos.v[0] += vel.v[0] * dt;
                pos.v[1] += vel.v[1] * dt;
                pos.v[2] += vel.v[2] * dt;
            }

            // compute accelerations for each body
            f_force(bodies);

            // update velocities and positions (kick)
            for (size_t i = 0; i < num_bodies; i++) {
                auto &vel = bodies[i].vel;
                auto &acc = bodies[i].acc;
                auto &pos = bodies[i].pos;

                // half velocity step
                vel.v[0] += acc.v[0] * half_dt;
                vel.v[1] += acc.v[1] * half_dt;
                vel.v[2] += acc.v[2] * half_dt;
            }
        }
    }

    // TODO: use proper logging system
    std::cout << "\rSimulated 100% of steps.            \n";

    // return recorded states with RVO
    return states;
}

}  // namespace nbodysim