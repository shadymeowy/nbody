#pragma once

// octree data structure and related functions
// core of the Barnes-Hut algorithm

// we hold everything in simple structs
// and store them in a vector hence no dynamic memory management
// this also allows really easy testing and consistency checks

#include <array>
#include <cassert>
#include <cstdint>
#include <msgpack.hpp>
#include <vector>

#include "common/body.hpp"
#include "common/vec.hpp"

namespace nbodysim {

// "bounding" cube structure
// (hoping it is the correct term)
struct BCube {
    Vec3 center{0.0, 0.0, 0.0};
    double half_size{0.0};

    BCube() = default;
    BCube(const Vec3 &c, double hs) : center(c), half_size(hs) {}

    // msgpack definition
    MSGPACK_DEFINE(center, half_size);
};

// node for octree
struct Node {
    // "bounding" cube
    BCube cube;
    // center of mass of bodies during evaluation
    // momentum info during insertion
    Vec3 cm{0.0, 0.0, 0.0};
    // total mass of bodies
    double mass{0.0};
    // child nodes
    std::array<int32_t, 8> children{{-1, -1, -1, -1, -1, -1, -1, -1}};
    // index of body if leaf node
    int32_t body_index{-1};
};

class Octree {
   public:
    // default constructor, creates empty octree
    explicit Octree() = default;

    // constructor with theta parameter
    explicit Octree(double theta) : theta2_{theta * theta} {}

    // calculate forces on bodies using the octree
    // updates bodies' inplace
    auto calculateForces(std::vector<Body> &bodies, bool reset = true) -> void;

    // act as a functor to calculate forces
    auto operator()(std::vector<Body> &bodies, bool reset = true) -> void {
        calculateForces(bodies, reset);
    }

    // expose nodes for testing and visualization
    const std::vector<Node> &getNodes() const {
        return nodes_;
    }

    // build octree from bodies without calculating forces
    void build(const std::vector<Body> &bodies);

   private:
    // storage for octree nodes
    // this is only used for caching during force calculation
    // after calculating forces, it is cleared
    std::vector<Node> nodes_;
    // barnes-hut theta parameter squared
    double theta2_{0.25};

    // insert first body as root node of octree
    auto insertRoot(const std::vector<Body> &bodies, int32_t body_idx) -> void;

    // insert a body into the octree starting from a given node
    // call it repeatedly to insert all bodies
    auto insertBody(const std::vector<Body> &bodies, int32_t body_idx,
                    int32_t node_idx = 0) -> void;

    // insert a child node into the octree by which octant it belongs to
    auto insertChild(Node &node, const Body &body, int32_t body_idx,
                     uint32_t octant) -> int32_t;

    // since we are storing center_of_mass as sum(pos*mass)
    // we need to finalize it after all insertions
    auto finalizeNodes() -> void;

    // calculate gravitational force exerted by a node on a body
    // using Barnes-Hut approximation
    auto calculateForceOnBody(std::vector<Body> &bodies, int32_t body_idx,
                              int32_t node_idx, double theta2 = 0.25) const
        -> void;

    // calculate the bounding cube size for given bodies
    // instead of calculating min/max for each axis,
    // we just find the maximum absolute coordinate
    static auto cubeBounds(const std::vector<Body> &bodies,
                           double padding = 1e-5) -> BCube;
};

// determine the octant index
static inline auto getOctant(Vec3 node_center, Vec3 body_pos) -> uint32_t {
    // first bit: x, second bit: y, third bit: z
    // if bit is high, body is in positive half along that axis
    uint32_t idx = 0;
    if (body_pos.v[0] >= node_center.v[0]) {
        idx |= 1U;
    }
    if (body_pos.v[1] >= node_center.v[1]) {
        idx |= 2U;
    }
    if (body_pos.v[2] >= node_center.v[2]) {
        idx |= 4U;
    }
    return idx;
}

}  // namespace nbodysim