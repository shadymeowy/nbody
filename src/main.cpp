#include <cstdlib>
#include <iostream>
#include <random>

#include "Common/body.h"
#include "body.h"

constexpr uint32_t BODY_COUNT = 10;
constexpr double MASS_FACTOR = 1e1;
constexpr double X_MAX = 100.l;
constexpr double Y_MAX = 100.l;
constexpr double Z_MAX = 100.l;
constexpr uint32_t SEED = 100;
constexpr size_t BODY_ID = 1;

void debug_print_body(const Body &body) {
    std::cout << "Body#" << body.id << ": Mass: " << std::scientific
              << body.mass << ", Pos: (" << std::scientific << body.pos.v[0]
              << "," << std::scientific << body.pos.v[1] << ","
              << std::scientific << body.pos.v[2] << "), Vel: ("
              << std::scientific << body.vel.v[0] << "," << std::scientific
              << body.vel.v[1] << "," << std::scientific << body.vel.v[2]
              << std::endl;
}

void accelerate_body(Body &body, const Vec3 &acceleration,
                     const double dt) noexcept {
    body.vel.v[0] += acceleration.v[0] * dt;
    body.vel.v[1] += acceleration.v[1] * dt;
    body.vel.v[2] += acceleration.v[2] * dt;
    body.pos.v[0] += body.vel.v[0] * dt;
    body.pos.v[1] += body.vel.v[1] * dt;
    body.pos.v[2] += body.vel.v[2] * dt;
}

void simulate_brute_force(std::vector<Body> &bodies, const double dt) noexcept {
    // TODO(Doruk): this can be allocated once and reused continuously
    std::vector accelerations(bodies.size(), Vec3{});
    for (const auto &body : bodies) {
        auto &body_acc = accelerations[body.id];
        for (const auto &other_body : bodies) {
            if (other_body.id == body.id) {
                continue;
            }
            Vec3 curr_attraction = calculate_attraction(body, other_body);
            body_acc.v[0] += curr_attraction.v[0];
            body_acc.v[1] += curr_attraction.v[1];
            body_acc.v[2] += curr_attraction.v[2];
        }
    }

    for (auto &body : bodies) {
        auto &body_acc = accelerations[body.id];
        // if (body.id == BODY_ID) {
        //     std::cout << "Body#" << BODY_ID << ": Acceleration: ("
        //               << std::scientific << body_acc.v[0] << ","
        //               << std::scientific << body_acc.v[1] << ","
        //               << std::scientific << body_acc.v[2] << ")" << "\n";
        // }
        accelerate_body(body, body_acc, dt);
    }
}

struct ProgramArgs {
    std::string strategy;
    uint32_t step_count;
    double timestep;
    uint32_t seed;
};

ProgramArgs parse_args(const int argc, char **argv) {
    ProgramArgs program_args{
        .strategy = "BarnesHut", .step_count = 500, .timestep = 0.1, .seed = 0};

    for (int i = 1; i < argc; i++) {
        if (strcmp("--strategy", argv[i]) == 0) {
            const char *strategy_arg = argv[i + 1];
            program_args.strategy = strategy_arg;
            if (program_args.strategy != "BruteForce" &&
                program_args.strategy != "BarnesHut") {
                std::cerr << "program strategy must be \"BruteForce\" or "
                             "\"BarnesHut\""
                          << std::endl;
                exit(1);
            }
        }
        if (strcmp("--stepcount", argv[i]) == 0) {
            const char *step_count_arg = argv[i + 1];
            program_args.step_count = std::stoi(step_count_arg);
            if (program_args.step_count <= 0) {
                std::cerr << "step count must be higher than 0" << std::endl;
                exit(1);
            }
        }
        if (strcmp("--seed", argv[i]) == 0) {
            const char *seed_arg = argv[i + 1];
            program_args.step_count = std::stoi(seed_arg);
            if (program_args.step_count <= 0) {
                std::cerr << "step count must be higher than 0" << std::endl;
                exit(1);
            }
        }
        if (strcmp("--timestep", argv[i]) == 0) {
            const char *timestep_arg = argv[i + 1];
            program_args.timestep = std::stod(timestep_arg);
            if (program_args.timestep <= 0) {
                std::cerr << "timestep must be higher than 0.0" << std::endl;
                exit(1);
            }
        }
    }

    return program_args;
}

void run_brute_force_simulation(std::vector<Body> bodies, const double dt,
                                const uint32_t step_count) {
    for (int32_t i = 0; i < step_count; i++) {
        simulate_brute_force(bodies, dt);
        const auto &body = bodies[BODY_ID];
        debug_print_body(body);
    }
}

struct Distributions {
   private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> _angle, _mass, _velocity, _z,
        _distance;

   public:
    explicit Distributions(const uint32_t seed) : rng(seed) {
        _angle = std::uniform_real_distribution<double>(0, 2 * M_PI);
        _distance = std::uniform_real_distribution<double>(2e11, 5e11);
        _mass = std::uniform_real_distribution<double>(1e15, 1e20);
        _velocity = std::uniform_real_distribution<double>(-1e4, 1e4);
        _z = std::uniform_real_distribution<double>(-1e10, 1e10);
    }

    double angle() { return _angle(rng); }
    double velocity() { return _velocity(rng); }
    double distance() { return _distance(rng); }
    double mass() { return _mass(rng); }
    double z() { return _z(rng); }
};

/*
 *TODO(Doruk): we can move these kind of templates to its own file/directory
 *later
 */
void generate_solar_system(Distributions distributions,
                           std::vector<Body> &bodies) {
    bodies = std::vector(40, Body{});
    // Sun
    bodies[0] = Body{.id = 0,
                     .mass = 1.989e30,
                     .pos = Vec3{0, 0, 0},
                     .vel = Vec3{0, 0, 0},
                     .radius = 6.96e8};  // meters

    // Mercury
    bodies[1] = Body{.id = 1,
                     .mass = 3.30e23,
                     .pos = Vec3{5.79e10, 0, 0},  // 57.9 million km in meters
                     .vel = Vec3{0, 4.74e4, 0},
                     .radius = 2.44e6};

    // Venus
    bodies[2] = Body{.id = 2,
                     .mass = 4.87e24,
                     .pos = Vec3{1.082e11, 0, 0},  // 108.2 million km in meters
                     .vel = Vec3{0, 3.5e4, 0},
                     .radius = 6.05e6};

    // Earth
    bodies[3] = Body{.id = 3,
                     .mass = 5.97e24,
                     .pos = Vec3{1.496e11, 0, 0},  // 1 AU in meters
                     .vel = Vec3{0, 2.98e4, 0},
                     .radius = 6.37e6};

    // Mars
    bodies[4] = Body{.id = 4,
                     .mass = 6.42e23,
                     .pos = Vec3{2.279e11, 0, 0},
                     .vel = Vec3{0, 2.41e4, 0},
                     .radius = 3.39e6};

    // Jupiter
    bodies[5] = Body{.id = 5,
                     .mass = 1.90e27,
                     .pos = Vec3{7.785e11, 0, 0},
                     .vel = Vec3{0, 1.31e4, 0},
                     .radius = 6.99e7};

    // Saturn
    bodies[6] = Body{.id = 6,
                     .mass = 5.68e26,
                     .pos = Vec3{1.432e12, 0, 0},
                     .vel = Vec3{0, 9.7e3, 0},
                     .radius = 5.82e7};

    // Uranus
    bodies[7] = Body{.id = 7,
                     .mass = 8.68e25,
                     .pos = Vec3{2.867e12, 0, 0},
                     .vel = Vec3{0, 6.8e3, 0},
                     .radius = 2.54e7};

    // Neptune
    bodies[8] = Body{.id = 8,
                     .mass = 1.02e26,
                     .pos = Vec3{4.515e12, 0, 0},
                     .vel = Vec3{0, 5.4e3, 0},
                     .radius = 2.46e7};

    // Pluto
    bodies[9] = Body{.id = 9,
                     .mass = 1.30e22,
                     .pos = Vec3{5.906e12, 0, 0},
                     .vel = Vec3{0, 4.7e3, 0},
                     .radius = 1.19e6};

    for (int i = 10; i < 40; i++) {
        const double angle = distributions.angle();
        const double dist = distributions.distance();

        bodies[i] = Body{
            .id = static_cast<uint32_t>(i),
            .mass = distributions.mass(),
            .pos =
                Vec3{dist * cos(angle), dist * sin(angle), distributions.z()},
            .vel = Vec3{distributions.velocity(), distributions.velocity(), 0},
            .radius = 1e4};
    }
}
int main(int argc, char **argv) {
    const ProgramArgs program_args = parse_args(argc, argv);
    Distributions distributions(SEED);

    std::vector<Body> bodies;
    generate_solar_system(distributions, bodies);

    constexpr double dt = 36;
    // from observation it seemed better!
    float theta = 0.7;
    if (program_args.strategy == "BruteForce") {
        run_brute_force_simulation(bodies, program_args.timestep,
                                   program_args.step_count);
    } else if (program_args.strategy == "BarnesHut") {
        BarnesHutApproximation(bodies, program_args.timestep, theta,
                               program_args.step_count);
    } else {
        std::cerr << "no strategy was chosen (BruteForce or BarnesHut)"
                  << std::endl;
        exit(1);
    }
}
