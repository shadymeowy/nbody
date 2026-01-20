#pragma once

// implements symplectic "yoshida 4th order integrator"
// generic over force calculation function (not force function itself, yet)

// yoshida integration is a symplectic integrator which is similar to velocity
// verlet but of order 4 instead of order 2. it requires 3 force calculations
// per step.

#include <spdlog/spdlog.h>

#include <cmath>
#include <cstddef>
#include <functional>
#include <vector>

#include "common/body.hpp"
#include "common/sim_state.hpp"

namespace nbodysim {

template <typename F, typename R>
auto simulateYoshida(std::vector<Body> &bodies, size_t n_steps, double dt,
                     size_t output_interval, F &&f, R &&r)
    -> SimResult {
    // get number of bodies and steps
    const size_t num_bodies = bodies.size();

    // half dt and dt for convenience
    const double half_dt = dt / 2.0;

    // coefficients for 4th order yoshida integrator
    // helper constants
    const double cqrt = std::pow(2.0, 1.0 / 3.0);
    const double w0 = -cqrt / (2.0 - cqrt);
    const double w1 = 1.0 / (2.0 - cqrt);

    // position update coefficients
    const std::array<double, 4> c_coeffs = {
        w1 / 2.0,
        (w0 + w1) / 2.0,
        (w0 + w1) / 2.0,
        w1 / 2.0,
    };

    // velocity update coefficients
    const std::array<double, 4> d_coeffs = {
        0.0,
        w1,
        w0,
        w1,
    };

    // initialize simulation state
    std::vector<SimState> states;
    states.reserve(n_steps);

    for (size_t block = 0; block < n_steps; block += output_interval) {
        const double current_time = static_cast<double>(block) * dt;

        // record current state
        states.emplace_back(current_time, bodies);

        // print progress
        std::invoke<R>(std::forward<R>(r), block);

        // perform output_interval steps
        // hope is that compiler will optimize this loop well
        for (size_t step = 0; step < output_interval; step++) {
            for (size_t k = 0; k < 4; k++) {
                // update velocities
                for (size_t i = 0; i < num_bodies; i++) {
                    auto &pos = bodies[i].pos;
                    auto &vel = bodies[i].vel;
                    auto &acc = bodies[i].acc;

                    vel.v[0] += d_coeffs[k] * acc.v[0] * dt;
                    vel.v[1] += d_coeffs[k] * acc.v[1] * dt;
                    vel.v[2] += d_coeffs[k] * acc.v[2] * dt;

                    pos.v[0] += c_coeffs[k] * vel.v[0] * dt;
                    pos.v[1] += c_coeffs[k] * vel.v[1] * dt;
                    pos.v[2] += c_coeffs[k] * vel.v[2] * dt;
                }

                // compute accelerations for each body
                if (k < 3) {
                    std::invoke<F>(std::forward<F>(f), bodies);
                }
            }
        }
    }

    // return recorded states with RVO
    return SimResult{states};
}

}  // namespace nbodysim