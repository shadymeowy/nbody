#pragma once

// project includes
#include "args/args.hpp"
#include "integrator/sim_state.hpp"
#include "viz/viz.hpp"

// CLI application class for nbody simulation
// used for both standard and web builds

// handles argument parsing and simulation execution
class CLIApp {
   public:
    // initialize CLI application with command line arguments
    explicit CLIApp(int argc, char **argv);

    // parsed arguments and simulation result
    nbodysim::Arguments args;
    nbodysim::SimResult result;

    // viz app
    std::unique_ptr<nbodysim::VizApp> viz_app{nullptr};

   private:
    auto simulateOrLoad() -> nbodysim::SimResult;
    auto simulate() -> nbodysim::SimResult;
};
