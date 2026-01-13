#pragma once

// octree data structure and related functions
// core of the Barnes-Hut algorithm

// we hold everything in simple structs
// and store them in a vector hence no dynamic memory management
// this also allows really easy testing and consistency checks

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

#include "common/body.hpp"
#include "common/vec.hpp"

namespace nbodysim {

// "bounding" cube structure
// (hoping it is the correct term)
struct BCube {
    Vec3 center{0.0, 0.0, 0.0};
    double half_size{0.0};
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

// insert first body as root node of octree
void insertRoot(std::vector<Node> &nodes, const std::vector<Body> &bodies,
                int32_t body_idx);

// insert a body into the octree starting from a given node
// call it repeatedly to insert all bodies
void insertBody(std::vector<Node> &nodes, const std::vector<Body> &bodies,
                int32_t body_idx, int32_t node_idx = 0);

// since we are storing center_of_mass as sum(pos*mass)
// we need to finalize it after all insertions
void finalizeNodes(std::vector<Node> &nodes);

// calculate gravitational force exerted by a node on a body
// using Barnes-Hut approximation
void calculateForceOnBody(const std::vector<Node> &nodes,
                          std::vector<Body> &bodies, int32_t body_idx,
                          int32_t node_idx, double theta2 = 0.25);

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