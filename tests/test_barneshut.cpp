#include <cstddef>
#include <cstdlib>
#include <iterator>
#include <vector>

#include "common/body.hpp"
#include "doctest.h"
#include "integrator/simstate.hpp"
#include "integrator/verlet.hpp"
#include "scenario/cluster.hpp"
#include "scenario/solar.hpp"
#include "space/barneshut.hpp"
#include "space/bruteforce.hpp"
#include "utils.hpp"

namespace nb = nbodysim;

TEST_CASE("solar system barneshut vs brute force") {
    // load solar system bodies
    std::vector<nb::Body> bodies(std::begin(nb::solar::bodies_j2000),
                                 std::end(nb::solar::bodies_j2000));

    // simulation parameters
    const int n_steps = 1000;
    const double timestep = 1e-6;
    const size_t output_interval = 10;

    // run brute force simulation
    auto states_bf = nb::simulateVerlet(bodies, n_steps, timestep,
                                        output_interval, nb::calculateForcesBF);

    // run barnes-hut simulation
    nb::BarnesHut barnes_hut{0.5};
    auto states_bh = nb::simulateVerlet(bodies, n_steps, timestep,
                                        output_interval, barnes_hut);

    // compare final states
    const auto &final_bf = states_bf.back();
    const auto &final_bh = states_bh.back();
    REQUIRE(final_bf.bodies.size() == final_bh.bodies.size());

    for (size_t i = 0; i < final_bf.bodies.size(); ++i) {
        const auto &body_bf = final_bf.bodies[i];
        const auto &body_bh = final_bh.bodies[i];

        // compare positions
        REQUIRE(isclose3(body_bf.pos, body_bh.pos, 1e-2, 1e-4));
    }
}

TEST_CASE("cluster barneshut vs brute force") {
    // load solar system bodies
    std::vector<nb::Body> bodies = nb::generateRandomCluster(100);

    // simulation parameters
    const int n_steps = 1000;
    const double timestep = 1e-6;
    const size_t output_interval = 10;

    // run brute force simulation
    auto states_bf = nb::simulateVerlet(bodies, n_steps, timestep,
                                        output_interval, nb::calculateForcesBF);

    // run barnes-hut simulation
    nb::BarnesHut barnes_hut{0.5};
    auto states_bh = nb::simulateVerlet(bodies, n_steps, timestep,
                                        output_interval, barnes_hut);

    // compare final states
    const auto &final_bf = states_bf.back();
    const auto &final_bh = states_bh.back();
    REQUIRE(final_bf.bodies.size() == final_bh.bodies.size());

    for (size_t i = 0; i < final_bf.bodies.size(); ++i) {
        const auto &body_bf = final_bf.bodies[i];
        const auto &body_bh = final_bh.bodies[i];

        // compare positions
        REQUIRE(isclose3(body_bf.pos, body_bh.pos, 1e-2, 1e-4));
    }
}
