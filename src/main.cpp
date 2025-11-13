#include <iostream>
#include <random>

#include "body.h"

struct Body;
constexpr int BODY_COUNT = 100;

constexpr double_t G = 100;

int main(int argc, char **argv) {
    auto bodies = std::vector<Body>(BODY_COUNT);
    for (uint32_t i = 0; i < BODY_COUNT; i++) {
        const double_t mass = std::rand() / RAND_MAX;
        bodies[i] = Body{
            .id = i,
            .mass = mass,
            .pos = Vec3{.x = 0.0l, .y = 0.0l, .z = 0.0l},
            .vel = Vec3{.x = 0.0l, .y = 0.0l, .z = 0.0l},
        };
    }

    const std::string hello_world = "Hello World!";
    std::cout << hello_world << std::endl;
}
