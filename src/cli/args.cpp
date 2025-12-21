#include "args.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

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
        {"VERLET", Arguments::Integrator::VERLET}};

    // add options
    app.add_option("--strategy", args.strategy, "Simulation strategy")
        ->transform(CLI::CheckedTransformer(strategy_map, CLI::ignore_case))
        ->capture_default_str();

    app.add_option("--scenario", args.scenario, "Initial condition scenario")
        ->transform(CLI::CheckedTransformer(scenario_map, CLI::ignore_case))
        ->required();

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

    app.add_option("--output", args.output, "Output file path (CSV)");

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
    spdlog::info("  Strategy: {}",  fmt::streamed(strategy));
    spdlog::info("  Scenario: {}", fmt::streamed(scenario));
    spdlog::info("  Integrator: {}", fmt::streamed(integrator));
    spdlog::info("  Duration: {} years", duration);
    spdlog::info("  Timestep: {} years", timestep);
    spdlog::info("  Output Interval: {} years", output_interval);
    spdlog::info("  Zero Momentum: {}", (nozmom ? "Disabled" : "Enabled"));
    spdlog::info("  Number of Bodies: {}", num_bodies);
    spdlog::info("  Seed: {}", seed);
    spdlog::info("  Output File: {}", (output.empty() ? "None" : output));
    spdlog::info("-----------------------------");
}

}  // namespace nbodysim