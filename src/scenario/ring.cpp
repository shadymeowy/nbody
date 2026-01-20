#include "ring.hpp"

#include <cmath>
#include <cstdint>
#include <random>
#include <vector>

#include "common/body.hpp"
#include "common/constants.hpp"
#include "common/energy_utils.hpp"

namespace nbodysim {

auto generateRandomRing(int num_bodies, uint32_t seed, double radius_au,
                        double width_au, double z_width, double mass_center,
                        double mass_astroid) -> std::vector<Body> {
    std::vector<Body> ring;
    ring.reserve(num_bodies + 1);

    // add central massive body
    Body central_body;
    central_body.pos = {0.0, 0.0, 0.0};
    central_body.vel = {0.0, 0.0, 0.0};
    central_body.mass = mass_center;
    ring.push_back(central_body);

    // TODO: box-muller
    // distribution various properties of bodies
    std::mt19937 gen(seed);
    std::normal_distribution<> dist_radius(radius_au, width_au);
    std::uniform_real_distribution<> dist_angle(0.0, 2.0 * constants::pi);
    std::uniform_real_distribution<> dist_mass(0.1 * mass_astroid,
                                               mass_astroid);
    std::uniform_real_distribution<> dist_z(-z_width * 0.5, z_width * 0.5);

    for (int i = 0; i < num_bodies; ++i) {
        Body b;

        // calculate position in ring
        const double r = dist_radius(gen);
        const double angle = dist_angle(gen);
        const double x = r * std::cos(angle);
        const double y = r * std::sin(angle);
        const double z = dist_z(gen);
        b.pos = {x, y, z};

        // mass of body
        b.mass = dist_mass(gen);

        // calculate orbital velocity
        const double v_mag = std::sqrt(constants::g_au * central_body.mass / r);
        const double vx = -v_mag * std::sin(angle);
        const double vy = v_mag * std::cos(angle);

        b.vel.v[0] = vx;
        b.vel.v[1] = vy;
        b.vel.v[2] = 0.0;

        ring.push_back(b);
    }

    // apply scaling to satisfy Virial Theorem
    // ie no exploding or collapsing ring
    virialScale(ring);

    return ring;
}

}  // namespace nbodysim