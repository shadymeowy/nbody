#include "cluster.hpp"

#include <cstdint>
#include <random>
#include <vector>

#include "common/body.hpp"
#include "common/energyutils.hpp"

namespace nbodysim {

// TODO: get more interesting distributions for cluster
auto generateRandomCluster(const int num_bodies, const uint32_t seed, const double radius_au)
    -> std::vector<Body> {
    std::vector<Body> cluster;
    cluster.reserve(num_bodies);

    std::mt19937 gen(seed);

    // distribution for position
    // TODO: this is REALLY slow for large number of bodies
    // we need to use box-muller method or similar asap
    std::normal_distribution<> dist_pos(0.0, radius_au * 0.5);

    // distribution for mass
    std::exponential_distribution<> dist_mass(100.0);

    for (int i = 0; i < num_bodies; ++i) {
        Body b;

        // draw random position in a cube
        b.pos.v[0] = dist_pos(gen);
        b.pos.v[1] = dist_pos(gen);
        b.pos.v[2] = dist_pos(gen);

        // draw random mass
        b.mass = 0.001 + (dist_mass(gen) * 0.05);

        const double spin = 0.5;
        b.vel.v[0] = -b.pos.v[1] * spin;
        b.vel.v[1] = b.pos.v[0] * spin;
        b.vel.v[2] = 0.0;

        cluster.push_back(b);
    }

    // apply scaling to satisfy Virial Theorem
    // ie no exploding or collapsing cluster
    virialScale(cluster);

    return cluster;
}

}  // namespace nbodysim