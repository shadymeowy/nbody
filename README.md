# IN1503 - Advanced Programming (Group 25)
This project is a simulation of an N-body system using Barnes-Hut algorithm for efficient computation of inverse square forces.

<img src="assets/demo.gif" alt="Simulation demo" width="600"/>

## Features
- Barnes-Hut algorithm using a octree structure for efficient force calculations
- Brute-force method for comparison and validation
- Verlet velocity integration for stable time evolution
- Fallback Euler's method for validation
- CLI for easy configuration of simulation parameters
- Different scenarios including clusters, rings, and NASA J2000 data for solar system
- Energy conservation checks
- Optional momentum zeroing at initialization for drift prevention
- Configurable output intervals and progress reporting
- Visualization using OpenGL via glviskit everywhere possible (even your mobile browser!)
- msgpack and CSV output for post-processing and analysis
- Visualize your results (msgpack/CSV) without re-running the simulation

See below for **Sprint Requirements** and details for easy navigation and checking off completed tasks.

## Requirements
- Any C++20 compliant compiler
- CMake 3.10 or higher

## Building the Project
1. Clone the repository for sprint2 branch:
```bash
git clone -b sprint2 https://gitlab.lrz.de/advprog2025/25.git
# or use SSH
git clone -b sprint2 git@gitlab.lrz.de:advprog2025/25.git
cd 25
```
2. Use cmake to build and test the project:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
ctest --test-dir build --output-on-failure
```

## Usage

Run the simulator via the `nbody` executable produced in `./build`:

```bash
./build/nbody [options]
```

### Examples

**Solar System (J2000) with brute-force strategy:**

```bash
./build/nbody --config config/j2000.toml --viz
```

**Ring scenario with Verlet integrator (100 years, 1024 bodies, timestep 0.1 years):**

```bash
./build/nbody --config config/ring.toml --viz
```

**Cluster scenario using Barnes–Hut octree strategy:**

```bash
./build/nbody --config config/cluster.toml --viz
```

## Requirements Checklist
### Sprint 1:
- [x] Generate an array of initial masses, positions, and velocities of a system of bodies in 3D space.
    - See `src/common/` and `src/scenario/`, configurable via `scenario` parameter in config files/CLI args.
- [x] Create a brute force n-body simulation O(n*n) 
    - See `src/space/brute_force.hpp`, configurable via `space` parameter in config files/CLI args.
- [x] Create a function for space-dividing an array of coordinates into an octree data structure.
    - See `src/space/octree.hpp` and `src/space/octree.cpp`.
- [x] Implement the Barnes-Hut algorithm for approximating n-body interactions using the octree.
    - See `src/space/octree.hpp`, configurable via `space` parameter in config files/CLI args.
- [x] Create a unit test, that compares the brute force reference solution with the Barnes-Hut approximation for a small test dataset.
    - See `tests/test_barnes_hut.cpp` and other tests in `tests/`.
- [x] Output a timeseries of the resulting positions of all bodies into a file 
    - See `src/integrator/sim_state.cpp` for CSV and MsgPack output functions, configurable via `csv_output` and `msgpack_output` parameters in config files/CLI args.

### Sprint 2:
- [x] Add the ability to specify parameters like the initial conditions, timestep size, simulation duration etc. in either a configuration file or as command line parameters.
    - See `src/cli/cli.cpp` and `config/` folder for configuration files, also settable via CLI args.
- [x] Create a class/datastructure that contains all the parameters (mass, position, velocity) of each body.
    - See `src/body/body.hpp` for Body class and `src/integrator/sim_state.hpp` for SimState and SimResult classes.
- [x] Add the ability to visualize the dynamics of the simulation in 3D space.
    - See `src/viz/` folder and `src/main_cli.cpp` for visualization using glviskit, configurable via `viz` and related parameters in config files/CLI args.
- [-] Abstract the interaction function of the bodies (and add an example of how to use it)
    - Partially done via `src/space/space.hpp` interface, no example yet with config.
- [x] Abstract the space dividing function.
    - See `src/space/` interface, for its usage see `src/integrator/euler.hpp`, `src/integrator/verlet.hpp` and `src/integrator/yoshida.hpp`.
- [x] Clean up and refactor the code.
    - Many code improvements, and refactoring throughout the codebase. OOP principles applied to various components where sprint 1 had more functional style.