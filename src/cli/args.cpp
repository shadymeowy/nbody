#include "args.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

// see args.hpp for details

namespace nbodysim {

auto Arguments::parse(int argc, char **argv) -> Arguments {
    Arguments args{};

    // First load positional and keyword arguments
    // we will probably not use positional args, but just in case
    std::vector<std::string> pargs;
    std::map<std::string, std::string> kwargs;
    for (int i = 1; i < argc; i++) {
        // check if starts with --
        const std::string arg = argv[i];
        if (arg.starts_with("--")) {
            // keyword argument
            const std::string &key = arg;
            std::string value;
            if (i + 1 < argc) {
                value = argv[i + 1];
                // skip next since it's value
                i++;
            } else {
                // no value provided
                value = "";
            }
            kwargs[key] = value;
        } else {
            // positional argument
            pargs.push_back(arg);
        }
    }

    // Now parse known arguments
    // If sth is not recognized, print help and exit
    for (const auto &[key, value] : kwargs) {
        if (key == "--strategy") {
            if (value == "BRUTE_FORCE") {
                args.strategy = Strategy::BRUTE_FORCE;
            } else if (value == "OCTREE") {
                args.strategy = Strategy::OCTREE;
            } else {
                Arguments::printHelp();
                std::cerr << "Unknown strategy: " + value << "\n";
                exit(1);
            }
        } else if (key == "--duration") {
            try {
                args.duration = std::stod(value);
            } catch (const std::invalid_argument &) {
                Arguments::printHelp();
                std::cerr << "Invalid duration: " + value << "\n";
                exit(1);
            }
        } else if (key == "--timestep") {
            try {
                args.timestep = std::stod(value);
            } catch (const std::invalid_argument &) {
                std::cerr << "Invalid timestep: " + value << "\n";
                exit(1);
            }
        } else if (key == "--output_interval") {
            try {
                args.output_interval = std::stod(value);
            } catch (const std::invalid_argument &) {
                Arguments::printHelp();
                std::cerr << "Invalid output interval: " + value << "\n";
                exit(1);
            }
        } else if (key == "--nozmom") {
            if (value == "true" || value == "1") {
                args.nozmom = true;
            } else if (value == "false" || value == "0") {
                args.nozmom = false;
            } else {
                Arguments::printHelp();
                std::cerr << "Invalid nozmom value: " + value << "\n";
                exit(1);
            }
        } else if (key == "--num_bodies") {
            try {
                args.num_bodies = static_cast<size_t>(std::stoul(value));
            } catch (const std::invalid_argument &) {
                Arguments::printHelp();
                std::cerr << "Invalid number of bodies: " + value << "\n";
                exit(1);
            }
        } else if (key == "--scenario") {
            if (value == "CLUSTER") {
                args.scenario = Scenario::CLUSTER;
            } else if (value == "J2000") {
                args.scenario = Scenario::J2000;
            } else if (value == "SIMPLE") {
                args.scenario = Scenario::SIMPLE;
            } else if (value == "RING") {
                args.scenario = Scenario::RING;
            } else {
                Arguments::printHelp();
                std::cerr << "Unknown scenario: " + value << "\n";
                exit(1);
            }
        } else if (key == "--integrator") {
            if (value == "EULER") {
                args.integrator = Integrator::EULER;
            } else if (value == "VERLET") {
                args.integrator = Integrator::VERLET;
            } else {
                Arguments::printHelp();
                std::cerr << "Unknown integrator: " + value << "\n";
                exit(1);
            }
        } else if (key == "--seed") {
            try {
                args.seed = static_cast<uint32_t>(std::stoul(value));
            } catch (const std::invalid_argument &) {
                Arguments::printHelp();
                std::cerr << "Invalid seed: " + value << "\n";
                exit(1);
            }
        } else if (key == "--output") {
            args.output = value;
        } else if (key == "--help") {
            // Print help and exit
            Arguments::printHelp();
            exit(0);
        } else {
            Arguments::printHelp();
            std::cerr << "Unknown argument: " + key << "\n";
            exit(1);
        }
    }

    // if any positional args are given, print help and exit
    if (!pargs.empty()) {
        Arguments::printHelp();
        std::cerr << "No positional arguments expected.\n";
        exit(1);
    }

    // if no scenario selected, print help and exit
    if (args.scenario == Scenario::NONE) {
        Arguments::printHelp();
        std::cerr << "No scenario selected.\n";
        exit(1);
    }

    return args;
}

void Arguments::printHelp() {
    // hack to get default values
    const Arguments default_args;

    std::cout << "Usage: nbody_simulator [options]\n";
    std::cout << "Options:\n";
    std::cout << "  --strategy [BRUTE_FORCE|OCTREE]\n"
              << "    Simulation strategy (default: BRUTE_FORCE)\n";
    std::cout << "  --scenario [CLUSTER|J2000|SIMPLE|RING]\n"
              << "    Initial condition scenario (default: J2000)\n";
    std::cout << "  --integrator [EULER|VERLET]\n"
              << "    Simulation integrator (default: VERLET)\n";
    std::cout << "  --duration [double]\n"
              << "    Duration of the simulation (default:"
              << default_args.duration << " years)\n";
    std::cout << "  --timestep [double]\n"
              << "    Time step size (default: " << default_args.timestep
              << " years)\n";
    std::cout << "  --output_interval [double]\n"
              << "    Interval for outputting data (default: "
              << default_args.output_interval << " years)\n";
    std::cout << "  --nozmom [bool]\n"
              << "    Disable zeroing of total momentum (default: "
              << (default_args.nozmom ? "true" : "false") << ")\n";
    std::cout
        << "  --num_bodies [size_t]\n"
        << "    Number of bodies to simulate in CLUSTER scenario (default: "
        << default_args.num_bodies << ")\n";
    std::cout << "  --seed [uint32_t]\n"
              << "    Random seed (default: " << default_args.seed << ")\n";
    std::cout << "  --output [string]\n"
              << "    Output file path\n";
    std::cout << "  --help\n"
              << "    Print this help message\n";

    // usage example (for j2000)
    std::cout << "\n";
    std::cout << "Example:\n";
    std::cout << "  nbody_simulator --strategy BRUTE_FORCE --scenario J2000 "
                 "--output output.csv\n";
    std::cout << "\n";
    std::cout
        << "This runs a brute-force simulation of the solar system "
           "and saves the output to output.csv, easiest way to get started.\n";
}

void Arguments::print() const {
    std::cout << "Simulation Arguments:\n";

    std::cout << "  Strategy: ";
    switch (strategy) {
        case Strategy::BRUTE_FORCE:
            std::cout << "BRUTE_FORCE\n";
            break;
        case Strategy::OCTREE:
            std::cout << "OCTREE\n";
            break;
    }

    std::cout << "  Scenario: ";
    switch (scenario) {
        case Scenario::CLUSTER:
            std::cout << "CLUSTER\n";
            break;
        case Scenario::J2000:
            std::cout << "J2000\n";
            break;
        case Scenario::SIMPLE:
            std::cout << "SIMPLE\n";
            break;
        case Scenario::RING:
            std::cout << "RING\n";
            break;
        case Scenario::NONE:
            std::cout << "NONE\n";
            break;
    }

    std::cout << "  Integrator: ";
    switch (integrator) {
        case Integrator::EULER:
            std::cout << "EULER\n";
            break;
        case Integrator::VERLET:
            std::cout << "VERLET\n";
            break;
    }

    std::cout << "  Duration: " << duration << " years\n";
    std::cout << "  Timestep: " << timestep << " years\n";
    std::cout << "  Output Interval: " << output_interval << " years\n";
    std::cout << "  Zero Momentum: " << (nozmom ? "Disabled" : "Enabled")
              << "\n";
    std::cout << "  Number of Bodies: " << num_bodies << "\n";
    std::cout << "  Seed: " << seed << "\n";
    std::cout << "  Output File: " << (output.empty() ? "None" : output)
              << "\n";
}

}  // namespace nbodysim