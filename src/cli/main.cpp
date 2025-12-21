// nbody simulation cli main file
// sets up and runs the simulation based on command line arguments
// supports different scenarios, strategies, and integrators
// for usage, run with --help

// it is the entry point of the program
// but we are mostly skipping details here
// so for details see other files
//     for argument parsing, args.hpp and args.cpp
//     for integrators, euler.hpp and verlet.hpp
//     for strategies, brute_force.hpp and barnes_hut.hpp
//     for scenarios, cluster.hpp and solar.hpp

#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <ios>
#include <iostream>
#include <vector>

// project includes
#include "cli/args.hpp"
#include "common/body.hpp"
#include "common/energy_utils.hpp"
#include "integrator/euler.hpp"
#include "integrator/sim_state.hpp"
#include "integrator/verlet.hpp"
#include "scenario/cluster.hpp"
#include "scenario/ring.hpp"
#include "scenario/solar.hpp"
#include "space/brute_force.hpp"
#include "space/octree.hpp"

// namespace alias for convenience
namespace nb = nbodysim;

auto main(int argc, char **argv) -> int {
    // parse arguments
    auto args = nb::Arguments::parse(argc, argv);

    // print arguments
    args.print();

    // use selected mode to initialize bodies
    std::vector<nb::Body> bodies;
    switch (args.scenario) {
        case nb::Arguments::Scenario::CLUSTER: {
            bodies = nb::generateRandomCluster(
                static_cast<int>(args.num_bodies), args.seed);
            break;
        }
        case nb::Arguments::Scenario::J2000: {
            bodies = std::vector<nb::Body>(std::begin(nb::solar::bodies_j2000),
                                           std::end(nb::solar::bodies_j2000));
            break;
        }
        case nb::Arguments::Scenario::SIMPLE: {
            bodies = std::vector<nb::Body>(std::begin(nb::solar::bodies_simple),
                                           std::end(nb::solar::bodies_simple));
            break;
        }
        case nb::Arguments::Scenario::RING: {
            bodies = nb::generateRandomRing(static_cast<int>(args.num_bodies),
                                            args.seed);
            break;
        }
        default: {
            std::cerr << "No valid scenario selected. Exiting.\n";
            return 1;
        }
    }

    std::cout << "Starting simulation with " << bodies.size() << " bodies.\n";

    // calculate number of steps and output interval
    auto n_steps = static_cast<size_t>(args.duration / args.timestep);
    auto output_interval =
        static_cast<size_t>(std::ceil(args.output_interval / args.timestep));

    std::cout << "Total steps: " << n_steps
              << ", Output interval: " << output_interval << " steps.\n";

    // zero momentum to avoid drift
    if (!args.nozmom) {
        std::cout << "Zeroing momentum to avoid drift.\n";
        nb::zeroMomentum(bodies);
    }

    // calculate initial energy
    const double initial_energy = nb::totalEnergy(bodies);
    std::cout << "Initial total energy: " << std::scientific << initial_energy
              << "\n";

    // start timer
    auto start = std::chrono::high_resolution_clock::now();

    // simulation states to save
    std::vector<nb::SimState> states;

    // get selected space strategy
    // note that we are only calling this once per simulation step
    // not per body, so overhead of std::function is acceptable
    std::function<void(std::vector<nb::Body> &)> space_strategy;
    switch (args.strategy) {
        case nb::Arguments::Strategy::BRUTE_FORCE: {
            space_strategy = nb::calculateForcesBF;
            break;
        }
        case nb::Arguments::Strategy::OCTREE: {
            // barnes-hut functor
            const nb::Octree octree{0.5};
            space_strategy = octree;
            break;
        }
    }

    // run selected integrator
    switch (args.integrator) {
        case nb::Arguments::Integrator::EULER: {
            states = nb::simulateEuler(bodies, n_steps, args.timestep,
                                       output_interval, space_strategy);
            break;
        }
        case nb::Arguments::Integrator::VERLET: {
            states = nb::simulateVerlet(bodies, n_steps, args.timestep,
                                        output_interval, space_strategy);
            break;
        }
    }

    // end timer
    auto end = std::chrono::high_resolution_clock::now();

    // print elapsed time
    const std::chrono::duration<double> elapsed = end - start;
    std::cout << "Simulation completed in " << elapsed.count() << " seconds.\n";
    std::cout << "Recorded " << states.size() << " simulation states.\n";

    // calculate final energy
    const double final_energy = nb::totalEnergy(bodies);
    std::cout << "Final total energy: " << std::scientific << final_energy
              << "\n";

    // calculate and print relative energy error
    const double rel_error =
        std::abs((final_energy - initial_energy) / initial_energy) * 100.0;
    std::cout << "Relative energy error (%): " << std::scientific << rel_error
              << "\n";

    // if output file is specified, save results
    if (!args.output.empty()) {
        std::cout << "Saving results to " << args.output << " ...\n";
        nb::saveSimStateToCSV(states, args.output);
    }

    return 0;
}