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

#include <spdlog/spdlog.h>

#include "cli.hpp"

auto main(int argc, char **argv) -> int {
    // create CLI application
    CLIApp app(argc, argv);

    // if visualization is enabled
    // call main loop of viz app
    if (app.viz_app) {
        app.viz_app->run();
    }
    return 0;
}