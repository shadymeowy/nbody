#include <cstdlib>
#include <iostream>
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

int main(int argc, char **argv) {
    std::mt19937 rng(SEED);
    std::uniform_real_distribution<double> dist(0, 1);
    std::vector bodies(2, Body{});
    bodies[0] = Body{.id = 0,
                     .mass = 5.97e24,
                     .pos =
                         Vec3{
                             -3.65e6,
                             0l,
                             0l,
                         },
                     .vel = Vec3{0l, -12.9l, 0l}};
    bodies[1] = Body{.id = 1,
                     .mass = 7.35e22,
                     .pos =
                         Vec3{
                             2.96e8,
                             0l,
                             0l,
                         },
                     .vel = Vec3{0l, 1050.0l, 0l}};

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

    for (const auto &body : bodies) {
        debug_print_body(body);
    }

    constexpr double dt = 1.0l / 60.0l;
    while (true) {
        simulate_brute_force(bodies, dt);
        const auto &body = bodies[BODY_ID];
        debug_print_body(body);
    }
}
