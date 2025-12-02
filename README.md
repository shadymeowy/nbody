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

## Requirements
- Any C++20 compliant compiler
- CMake 3.10 or higher
- Python 3.8 >= for visualization (will be removed in next sprint)
- OpenCV for video saving in visualization

## Building the Project
1. Clone the repositor for sprint1 branch:
```bash
git clone -b sprint1 https://gitlab.lrz.de/advprog2025/25.git
# or use SSH
git clone -b sprint1 git@gitlab.lrz.de:advprog2025/25.git
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

### Options

#### `--strategy [BRUTE_FORCE|OCTREE]`

Selects the force computation strategy.

* `BRUTE_FORCE`: (O(N^2)) exact pairwise forces (useful for validation).
* `OCTREE`: Barnes–Hut octree approximation (faster for large (N)).
* **Default:** `BRUTE_FORCE`

#### `--scenario [CLUSTER|J2000|SIMPLE|RING]`

Chooses the initial condition scenario.

* **Default:** `J2000`

#### `--integrator [EULER|VERLET]`

Selects the time integration scheme.

* `VERLET`: recommended for stability.
* `EULER`: mainly for validation/debugging.
* **Default:** `VERLET`

#### `--duration [double]`

Total simulated time in **years**.

* **Default:** `250`

#### `--timestep [double]`

Simulation step size in **years**.

* **Default:** `0.001`

#### `--output_interval [double]`

Interval (in **years**) at which state snapshots are written to the output file.

* **Default:** `0.1`

#### `--nozmom [bool]`

Disables zeroing of total momentum at initialization (useful for experiments; typically keep enabled behavior).

* **Default:** `false`
  *(i.e., momentum is zeroed unless this is set to `true`)*

#### `--num_bodies [size_t]`

Number of bodies for the `CLUSTER` scenario.

* **Default:** `16`

#### `--seed [uint32_t]`

Random seed for stochastic scenarios (e.g., `CLUSTER`).

* **Default:** `42`

#### `--output [string]` *(required)*

Path to the output `.csv` file.

#### `--help`

Prints the help message and exits.

### Examples

**Solar System (J2000) with brute-force strategy:**

```bash
./build/nbody --strategy BRUTE_FORCE --scenario J2000 --output j2000_output.csv
```

**Ring scenario with Verlet integrator (100 years, 1024 bodies, timestep 0.1 years):**

```bash
./build/nbody --timestep 1e-1 --output output.csv --integrator VERLET --scenario RING --duration 100 --num_bodies 1024
```

**Cluster scenario using Barnes–Hut octree strategy:**

```bash
./build/nbody --strategy OCTREE --timestep 1e-6 --scenario CLUSTER --output cluster_output.csv
```

## Visualization
Two different Python scripts are provided for visualization:
- `utils/plot.py`: Plots static graphs of the simulation data, only requires matplotlib.
- `utils/visualize.py`: Uses a real-time rendering window for dynamic visualization, requires Linux or MacOS, and OpenCV for video saving.