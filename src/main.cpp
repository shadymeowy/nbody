#include <cstdlib>
#include <iostream>
#include <random>
#include <random>
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
};

ProgramArgs parse_args(const int argc, char **argv) {
    ProgramArgs program_args = {};
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
    }

    return program_args;
}

void run_brute_force_simulation(std::vector<Body> bodies, const double dt) {
    while (true) {
        simulate_brute_force(bodies, dt);
        const auto &body = bodies[BODY_ID];
        debug_print_body(body);
    }
}

int main(int argc, char **argv) {
    const ProgramArgs program_args = parse_args(argc, argv);
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> dist(0, 1);
    std::vector bodies(40, Body{});
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> angle_dist(0, 2 * M_PI);
    std::uniform_real_distribution<double> dist_dist(2e11, 5e11);      // Between Mars & Jupiter
    std::uniform_real_distribution<double> mass_dist(1e15, 1e20);
    std::uniform_real_distribution<double> vel_dist(-1e4, 1e4);
    std::uniform_real_distribution<double> z_dist(-1e10, 1e10);

    // Sun
    bodies[0] = Body{.id = 0, .mass = 1.989e30,
                     .pos = Vec3{0, 0, 0},
                     .vel = Vec3{0, 0, 0}, .radius = 6.96e8};  // meters

    // Mercury
    bodies[1] = Body{.id = 1, .mass = 3.30e23,
                     .pos = Vec3{5.79e10, 0, 0},    // 57.9 million km in meters
                     .vel = Vec3{0, 4.74e4, 0}, .radius = 2.44e6};

    // Venus
    bodies[2] = Body{.id = 2, .mass = 4.87e24,
                     .pos = Vec3{1.082e11, 0, 0},   // 108.2 million km in meters
                     .vel = Vec3{0, 3.5e4, 0}, .radius = 6.05e6};

    // Earth
    bodies[3] = Body{.id = 3, .mass = 5.97e24,
                     .pos = Vec3{1.496e11, 0, 0},   // 1 AU in meters
                     .vel = Vec3{0, 2.98e4, 0}, .radius = 6.37e6};

    // Mars
    bodies[4] = Body{.id = 4, .mass = 6.42e23,
                     .pos = Vec3{2.279e11, 0, 0},
                     .vel = Vec3{0, 2.41e4, 0}, .radius = 3.39e6};

    // Jupiter
    bodies[5] = Body{.id = 5, .mass = 1.90e27,
                     .pos = Vec3{7.785e11, 0, 0},
                     .vel = Vec3{0, 1.31e4, 0}, .radius = 6.99e7};

    // Saturn
    bodies[6] = Body{.id = 6, .mass = 5.68e26,
                     .pos = Vec3{1.432e12, 0, 0},
                     .vel = Vec3{0, 9.7e3, 0}, .radius = 5.82e7};

    // Uranus
    bodies[7] = Body{.id = 7, .mass = 8.68e25,
                     .pos = Vec3{2.867e12, 0, 0},
                     .vel = Vec3{0, 6.8e3, 0}, .radius = 2.54e7};

    // Neptune
    bodies[8] = Body{.id = 8, .mass = 1.02e26,
                     .pos = Vec3{4.515e12, 0, 0},
                     .vel = Vec3{0, 5.4e3, 0}, .radius = 2.46e7};

    // Pluto
    bodies[9] = Body{.id = 9, .mass = 1.30e22,
                     .pos = Vec3{5.906e12, 0, 0},
                     .vel = Vec3{0, 4.7e3, 0}, .radius = 1.19e6};

    for (int i = 10; i < 40; i++) {
        double angle = angle_dist(gen);
        double dist = dist_dist(gen);

        bodies[i] = Body{.id = static_cast<unsigned int>(i),
            .mass = mass_dist(gen),
            .pos = Vec3{
                dist * cos(angle),
                dist * sin(angle),
                z_dist(gen)
            },
            .vel = Vec3{
                vel_dist(gen),
                vel_dist(gen),
                0
            },
            .radius = 1e4
        };
    }


    // uint32_t i = 0;
    // for (auto &body : bodies) {
    //     body = Body{.id = i,
    //                 .mass = MASS_FACTOR * dist(rng),
    //                 .pos =
    //                     Vec3{
    //                         dist(rng),
    //                         dist(rng),
    //                         dist(rng),
    //                     },
    //                 .vel = Vec3{0}};
    //     i++;
    // }

    constexpr double dt = 36;
    // from observation it seemed better!
    float theta = 0.7;
    int steps = 1000000;
    if (program_args.strategy == "BruteForce") {
        run_brute_force_simulation(bodies, dt);
    } else if (program_args.strategy == "BarnesHut") {
        BarnesHutApproximation(bodies, dt, theta, steps);

    } else {
        std::cerr << "no strategy was chosen (BruteForce or BarnesHut)"
                  << std::endl;
        exit(1);
    }
}
