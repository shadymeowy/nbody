#pragma once

// argument parsing for nbody simulation
// using CLI11 library as promised

#include <spdlog/spdlog.h>

#include <array>
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
    };
    enum class Integrator {
        EULER,
        VERLET,
        YOSHIDA,
    };

    // whether to enable debug logging
    bool debug = false;
    // "space handling" strategy
    Strategy strategy = Strategy::BRUTE_FORCE;
    // scenarios
    Scenario scenario = Scenario::RING;
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
    // output csv file path
    std::string csv_output;
    // output msgpack file path
    std::string msgpack_output;
    // input csv file path
    std::string csv_input;
    // input msgpack file path
    std::string msgpack_input;

    // visualization parameters
    struct Viz {
        // enable visualization
        bool enable = false;
        // whether to show orbits
        bool show_orbits = false;
        // speed factor for visualization
        float speed_factor = 1.0F;
        // trail length in years
        float trail_length = 1.0F;
        // number of segments in trail
        size_t trail_segments = 10;
        // size scaling factor
        float size_scale = 300.0F;
        // minimum body size
        float size_min = 2.0F;
        // maximum body size
        float size_max = 32.0F;
        // orbit line width
        float orbit_width = 3.0F;
        // camera center position
        std::array<float, 3> camera_center = {0.0F, 0.0F, 0.0F};
        // camera distance
        float camera_distance = 5.0F;
        // camera rotation (pitch, yaw, roll)
        std::array<float, 3> camera_rotation = {0.0F, 0.0F, 0.0F};
        // show octree
        bool show_octree = false;
    } viz;

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
        case Arguments::Integrator::YOSHIDA:
            return os << "YOSHIDA";
    }
    return os << "UNKNOWN";
}
}  // namespace nbodysim