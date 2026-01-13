#pragma once

// argument parsing for nbody simulation
// using simple custom parser
// (we would be using a proper library later sprints, pinky promise)

#include <cstddef>
#include <cstdint>
#include <string>

namespace nbodysim {

struct Arguments {
    enum class Strategy : std::uint8_t {
        BRUTE_FORCE,
        OCTREE,
    };
    enum class Scenario: std::uint8_t {
        CLUSTER,
        J2000,
        SIMPLE,
        RING,
        NONE,
    };
    enum class Integrator : std::uint8_t {
        EULER,
        VERLET,
    };

    // "space handling" strategy
    Strategy strategy = Strategy::BRUTE_FORCE;
    // scenarios
    Scenario scenario = Scenario::NONE;
    // integrator
    Integrator integrator = Integrator::VERLET;
    // simulation parameters
    double duration = 250.0;
    // timestep in years
    double timestep = 1e-3;
    // output interval in years
    double output_interval = 1e-1;
    // disable zeroing of total momentum at start
    bool nozmom = false;
    // number of bodies (for cluster scenario)
    size_t num_bodies = 16;
    // random seed for reproducibility
    uint32_t seed = 42;
    // output file path
    std::string output;

    // parse arguments from command line
    static auto parse(int argc, char **argv) -> Arguments;

    // print help message
    static void printHelp();

    // print current arguments
    void print() const;
};

}  // namespace nbodysim