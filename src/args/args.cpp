#include "args.hpp"

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <cstdlib>
#include <map>
#include <string>

// see args.hpp for details

namespace nbodysim {

auto Arguments::parse(int argc, char **argv) -> Arguments {
    Arguments args{};
    CLI::App app{"nbody - A simple n-body gravitational simulator",
                 "A simple n-body gravitational simulator implemented in C++ "
                 "for IN1503"};

    // enumeration mappings
    std::map<std::string, Arguments::Strategy> strategy_map{
        {"BRUTE_FORCE", Arguments::Strategy::BRUTE_FORCE},
        {"OCTREE", Arguments::Strategy::OCTREE}};

    std::map<std::string, Arguments::Scenario> scenario_map{
        {"CLUSTER", Arguments::Scenario::CLUSTER},
        {"J2000", Arguments::Scenario::J2000},
        {"SIMPLE", Arguments::Scenario::SIMPLE},
        {"RING", Arguments::Scenario::RING}};

    std::map<std::string, Arguments::Integrator> integrator_map{
        {"EULER", Arguments::Integrator::EULER},
        {"VERLET", Arguments::Integrator::VERLET},
        {"YOSHIDA", Arguments::Integrator::YOSHIDA}};

    // add options
    app.add_option("--strategy", args.strategy, "Simulation strategy")
        ->transform(CLI::CheckedTransformer(strategy_map, CLI::ignore_case))
        ->capture_default_str();

    app.add_option("--scenario", args.scenario, "Initial condition scenario")
        ->transform(CLI::CheckedTransformer(scenario_map, CLI::ignore_case))
        ->capture_default_str();

    app.add_option("--integrator", args.integrator, "Simulation integrator")
        ->transform(CLI::CheckedTransformer(integrator_map, CLI::ignore_case))
        ->capture_default_str();

    app.add_option("--duration", args.duration,
                   "Duration of the simulation (years)")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--timestep", args.timestep, "Time step size (years)")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--output_interval", args.output_interval,
                   "Interval for outputting data")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_flag("--nozmom", args.nozmom, "Disable zeroing of total momentum");

    app.add_option("--num_bodies", args.num_bodies,
                   "Number of bodies (CLUSTER scenario)")
        ->capture_default_str();

    app.add_option("--seed", args.seed, "Random seed for reproducibility")
        ->capture_default_str();

    app.add_option("--csv_output", args.csv_output, "Output CSV file name");
    app.add_option("--msgpack_output", args.msgpack_output,
                   "Output MsgPack file name");
    app.add_option("--csv_input", args.csv_input, "Input CSV file name");
    app.add_option("--msgpack_input", args.msgpack_input,
                   "Input MsgPack file name");

    // visualization options
    app.add_flag("--viz", args.viz.enable, "Enable visualization");

    app.add_flag("--viz_show_orbits", args.viz.show_orbits,
                 "Show orbits in visualization");

    app.add_option("--viz_speed_factor", args.viz.speed_factor,
                   "Speed factor in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_trail_length", args.viz.trail_length,
                   "Trail length in visualization (years)")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_trail_segments", args.viz.trail_segments,
                   "Number of trail segments in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_size_scale", args.viz.size_scale,
                   "Size scaling factor in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_size_min", args.viz.size_min,
                   "Minimum body size in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_size_max", args.viz.size_max,
                   "Maximum body size in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_orbit_width", args.viz.orbit_width,
                   "Orbit line width in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_camera_center", args.viz.camera_center,
                   "Camera center position in visualization (x,y,z)")
        ->expected(1)
        ->type_size(3)
        ->capture_default_str();

    app.add_option("--viz_camera_distance", args.viz.camera_distance,
                   "Camera distance in visualization")
        ->check(CLI::PositiveNumber)
        ->capture_default_str();

    app.add_option("--viz_camera_rotation", args.viz.camera_rotation,
                   "Camera rotation (pitch,yaw,roll) in visualization")
        ->expected(1)
        ->type_size(3)
        ->capture_default_str();

    // configuration file option
    app.set_config("--config")
        ->description("Read settings from a file")
        ->expected(1);

    // try to parse arguments
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        // on error, exit with appropriate code
        // this also handles --help and --version
        std::exit(app.exit(e));
    }

    args.print();
    return args;
}

auto Arguments::print() const -> void {
    spdlog::info("--- Simulation Arguments ---");
    spdlog::info("  Strategy: {}", fmt::streamed(strategy));
    spdlog::info("  Scenario: {}", fmt::streamed(scenario));
    spdlog::info("  Integrator: {}", fmt::streamed(integrator));
    spdlog::info("  Duration: {} years", duration);
    spdlog::info("  Timestep: {} years", timestep);
    spdlog::info("  Output Interval: {} years", output_interval);
    spdlog::info("  Zero Momentum: {}", (nozmom ? "Disabled" : "Enabled"));
    spdlog::info("  Number of Bodies: {}", num_bodies);
    spdlog::info("  Seed: {}", seed);
    spdlog::info("  Output CSV File: {}",
                 (csv_output.empty() ? "None" : csv_output));
    spdlog::info("  Output MsgPack File: {}",
                 (msgpack_output.empty() ? "None" : msgpack_output));
    spdlog::info("  Input CSV File: {}",
                 (csv_input.empty() ? "None" : csv_input));
    spdlog::info("  Input MsgPack File: {}",
                 (msgpack_input.empty() ? "None" : msgpack_input));
    spdlog::info("  Visualization: {}", (viz.enable ? "Enabled" : "Disabled"));
    if (viz.enable) {
        spdlog::info("    Show Orbits: {}", (viz.show_orbits ? "Yes" : "No"));
        spdlog::info("    Speed Factor: {}", viz.speed_factor);
        spdlog::info("    Trail Length: {} years", viz.trail_length);
        spdlog::info("    Trail Segments: {}", viz.trail_segments);
        spdlog::info("    Size Scale: {}", viz.size_scale);
        spdlog::info("    Size Min: {}", viz.size_min);
        spdlog::info("    Size Max: {}", viz.size_max);
        spdlog::info("    Orbit Width: {}", viz.orbit_width);
    }
    spdlog::info("-----------------------------");
}

}  // namespace nbodysim