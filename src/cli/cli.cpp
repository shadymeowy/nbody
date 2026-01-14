#include "cli.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <functional>
#include <indicators/cursor_control.hpp>
#include <indicators/progress_bar.hpp>
#include <iostream>
#include <vector>

// project includes
#include "args/args.hpp"
#include "common/body.hpp"
#include "common/energy_utils.hpp"
#include "integrator/euler.hpp"
#include "integrator/sim_state.hpp"
#include "integrator/verlet.hpp"
#include "integrator/yoshida.hpp"
#include "scenario/cluster.hpp"
#include "scenario/ring.hpp"
#include "scenario/solar.hpp"
#include "space/brute_force.hpp"
#include "space/octree.hpp"
#include "viz/viz.hpp"

// convenience namespace alias
namespace nb = nbodysim;

CLIApp::CLIApp(int argc, char **argv) : args(nb::Arguments::parse(argc, argv)) {
    // check if input file is specified
    // load simulation states from file if specified
    // otherwise run new simulation
    nb::SimResult result;
    if (!args.msgpack_input.empty() && args.msgpack_input != "") {
        spdlog::info("Loading simulation states from {} ...",
                     args.msgpack_input);
        result = nb::SimResult::loadFromMsgPack(args.msgpack_input);
        spdlog::info("Loaded {} simulation states.", result.states.size());
    } else if (!args.csv_input.empty() && args.csv_input != "") {
        spdlog::info("Loading simulation states from {} ...", args.csv_input);
        result = nb::SimResult::loadFromCSV(args.csv_input);
        spdlog::info("Loaded {} simulation states.", result.states.size());
    } else {
        spdlog::info("No input file specified. Running new simulation.");
        result = simulate();
    }

    // calculate initial energy
    const double initial_energy = nb::totalEnergy(result.states.front().bodies);
    spdlog::info("Initial total energy: {:e}", initial_energy);

    // calculate final energy
    const double final_energy = nb::totalEnergy(result.states.back().bodies);
    spdlog::info("Final total energy: {:e}", final_energy);

    // calculate and print relative energy error
    const double rel_error =
        std::abs((final_energy - initial_energy) / initial_energy) * 100.0;
    spdlog::info("Relative energy error (%): {:e}", rel_error);

    // if output csv file is specified, save results
    if (!args.csv_output.empty() && args.csv_output != "") {
        spdlog::info("Saving results to {} ...", args.csv_output);
        result.saveToCSV(args.csv_output);
    }

    // if output msgpack file is specified, save results
    if (!args.msgpack_output.empty() && args.msgpack_output != "") {
        spdlog::info("Saving results to {} ...", args.msgpack_output);
        result.saveToMsgPack(args.msgpack_output);
    }

    if (args.viz.enable) {
        spdlog::info("Starting visualization...");
        viz_app = std::make_unique<nb::VizApp>(result);

        // set visualization parameters
        viz_app->setShowOrbits(args.viz.show_orbits);
        viz_app->setSpeedFactor(args.viz.speed_factor);
        viz_app->setTrailLength(args.viz.trail_length);
        viz_app->setTrailSegments(args.viz.trail_segments);
        viz_app->setSizeScale(args.viz.size_scale);
        viz_app->setSizeMin(args.viz.size_min);
        viz_app->setSizeMax(args.viz.size_max);
        viz_app->setOrbitWidth(args.viz.orbit_width);
    }
}

auto CLIApp::simulate() -> nb::SimResult {
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
            std::exit(1);
        }
    }

    spdlog::info("Starting simulation with {} bodies.", bodies.size());

    // calculate number of steps and output interval
    auto n_steps = static_cast<size_t>(args.duration / args.timestep);
    auto output_interval =
        static_cast<size_t>(std::ceil(args.output_interval / args.timestep));

    spdlog::info("Total steps: {}, Output interval: {} steps.", n_steps,
                 output_interval);

    // zero momentum to avoid drift
    if (!args.nozmom) {
        spdlog::info("Zeroing momentum to avoid drift.");
        nb::zeroMomentum(bodies);
    }

    // start timer
    auto start = std::chrono::high_resolution_clock::now();

    // simulation states to save
    nb::SimResult result;

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

#ifndef EMSCRIPTEN
    // progress bar setup
    indicators::ProgressBar bar{
        indicators::option::BarWidth{50},
        indicators::option::Start{"["},
        indicators::option::End{"]"},
        indicators::option::ForegroundColor{indicators::Color::white},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true},
        indicators::option::MaxProgress{n_steps},
    };

    // lambda to update progress bar
    auto progress_updater = [&](size_t current_step) {
        bar.set_progress(current_step);
        if (current_step >= n_steps) {
            bar.mark_as_completed();
            std::cout << "\n" << std::flush;
        }
    };
#else
    // empty progress updater for web builds
    auto progress_updater = [](size_t) {};
#endif

    // run selected integrator
    switch (args.integrator) {
        case nb::Arguments::Integrator::EULER: {
            result = nb::simulateEuler(bodies, n_steps, args.timestep,
                                       output_interval, space_strategy,
                                       progress_updater);
            break;
        }
        case nb::Arguments::Integrator::VERLET: {
            result = nb::simulateVerlet(bodies, n_steps, args.timestep,
                                        output_interval, space_strategy,
                                        progress_updater);
            break;
        }
        case nb::Arguments::Integrator::YOSHIDA: {
            result = nb::simulateYoshida(bodies, n_steps, args.timestep,
                                         output_interval, space_strategy,
                                         progress_updater);
            break;
        }
    }

    // end timer
    auto end = std::chrono::high_resolution_clock::now();

    // print elapsed time
    const std::chrono::duration<double> elapsed = end - start;
    spdlog::info("Simulation completed in {} seconds.", elapsed.count());
    spdlog::info("Recorded {} simulation states.", result.states.size());

    return result;
}
