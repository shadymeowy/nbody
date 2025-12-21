#pragma once

// argument parsing for nbody simulation
// using simple custom parser
// (we would be using a proper library later sprints, pinky promise)

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

namespace nbodysim {

struct Arguments {
    enum class Strategy {
        BRUTE_FORCE,
        OCTREE,
    };
    enum class Scenario {
        CLUSTER,
        J2000,
        SIMPLE,
        RING,
        NONE,
    };
    enum class Integrator {
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

    // print arguments to console
    auto print() const -> void;
};

// overload stream operators for enums for easy printing
// TODO: maybe move to separate file?
inline auto operator<<(std::ostream &os, const Arguments::Strategy &s)
    -> std::ostream & {
    switch (s) {
        case Arguments::Strategy::BRUTE_FORCE:
            return os << "BRUTE_FORCE";
        case Arguments::Strategy::OCTREE:
            return os << "OCTREE";
    }
    return os << "UNKNOWN";
}

inline auto operator<<(std::ostream &os, const Arguments::Scenario &s)
    -> std::ostream & {
    switch (s) {
        case Arguments::Scenario::CLUSTER:
            return os << "CLUSTER";
        case Arguments::Scenario::J2000:
            return os << "J2000";
        case Arguments::Scenario::SIMPLE:
            return os << "SIMPLE";
        case Arguments::Scenario::RING:
            return os << "RING";
        default:
            return os << "NONE";
    }
}

inline auto operator<<(std::ostream &os, const Arguments::Integrator &i)
    -> std::ostream & {
    switch (i) {
        case Arguments::Integrator::EULER:
            return os << "EULER";
        case Arguments::Integrator::VERLET:
            return os << "VERLET";
    }
    return os << "UNKNOWN";
}
}  // namespace nbodysim