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