#include "octree.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "common/body.hpp"
#include "common/constants.hpp"
#include "common/vec.hpp"

namespace nbodysim {

namespace {
// calculate the bounding cube size for given bodies
// instead of calculating min/max for each axis,
// we just find the maximum absolute coordinate
auto cubeBounds(const std::vector<Body> &bodies, double padding = 1e-5)
    -> BCube {
    // initialize min and max vectors
    Vec3 mn{std::numeric_limits<double>::max(),
            std::numeric_limits<double>::max(),
            std::numeric_limits<double>::max()};

    Vec3 mx{std::numeric_limits<double>::lowest(),
            std::numeric_limits<double>::lowest(),
            std::numeric_limits<double>::lowest()};

    // find min and max coordinates by looping over
    for (const auto &b : bodies) {
        mn.v[0] = std::min(mn.v[0], b.pos.v[0]);
        mn.v[1] = std::min(mn.v[1], b.pos.v[1]);
        mn.v[2] = std::min(mn.v[2], b.pos.v[2]);
        mx.v[0] = std::max(mx.v[0], b.pos.v[0]);
        mx.v[1] = std::max(mx.v[1], b.pos.v[1]);
        mx.v[2] = std::max(mx.v[2], b.pos.v[2]);
    }

    // calculate center of the box
    const Vec3 center{
        (mn.v[0] + mx.v[0]) * 0.5,
        (mn.v[1] + mx.v[1]) * 0.5,
        (mn.v[2] + mx.v[2]) * 0.5,
    };

    // calculate the size along each axis
    const double x_size = mx.v[0] - mn.v[0];
    const double y_size = mx.v[1] - mn.v[1];
    const double z_size = mx.v[2] - mn.v[2];
    const double max_size = std::max({x_size, y_size, z_size});

    // add small padding to avoid boundary issues
    // ie bodies on the edge
    const double half_size = (max_size * 0.5) + padding;

    return BCube{.center = center, .half_size = half_size};
}

auto insertChild(std::vector<Node> &nodes, Node &node, const Body &body,
                 int32_t body_idx, uint32_t octant) -> int32_t {
    // ensure child does not exist, should not happen
    assert(node.children[octant] == -1);

    Node child{};
    // calculate child cube
    const double half_size = node.cube.half_size * 0.5;
    Vec3 offset{
        ((octant & 1U) != 0U) ? half_size : -half_size,
        ((octant & 2U) != 0U) ? half_size : -half_size,
        ((octant & 4U) != 0U) ? half_size : -half_size,
    };
    child.cube.center = Vec3{
        node.cube.center.v[0] + offset.v[0],
        node.cube.center.v[1] + offset.v[1],
        node.cube.center.v[2] + offset.v[2],
    };
    child.cube.half_size = half_size;
    // set body info
    child.body_index = body_idx;
    child.cm.v[0] = body.pos.v[0] * body.mass;
    child.cm.v[1] = body.pos.v[1] * body.mass;
    child.cm.v[2] = body.pos.v[2] * body.mass;
    child.mass = body.mass;

    // add child to nodes
    // first calculate its index then push back
    // else the ref will be invalidated
    auto child_idx = static_cast<int32_t>(nodes.size());
    node.children[static_cast<size_t>(octant)] = child_idx;
    nodes.push_back(child);
    return child_idx;
}

}  // namespace

void insertRoot(std::vector<Node> &nodes, const std::vector<Body> &bodies,
                int32_t body_idx) {
    // clear previous octree
    nodes.clear();

    // get the body to insert
    assert(body_idx >= 0 && body_idx < static_cast<int32_t>(bodies.size()));
    const Body &body = bodies[body_idx];

    // create root node
    Node root{};
    root.cube = cubeBounds(bodies);
    root.body_index = body_idx;

    // store momentum info instead of position*mass for center of mass
    root.cm.v[0] = body.pos.v[0] * body.mass;
    root.cm.v[1] = body.pos.v[1] * body.mass;
    root.cm.v[2] = body.pos.v[2] * body.mass;
    root.mass = body.mass;
    nodes.push_back(root);
}

void insertBody(std::vector<Node> &nodes, const std::vector<Body> &bodies,
                int32_t body_idx, int32_t node_idx) {
    // get the body to insert
    assert(body_idx >= 0 && body_idx < static_cast<int32_t>(bodies.size()));
    const Body &body = bodies[body_idx];

    // ensure nodes is not empty, atleast root node should exist
    assert(!nodes.empty());

    // get the current node
    assert(node_idx >= 0 && node_idx < static_cast<int32_t>(nodes.size()));
    Node &node = nodes[node_idx];

    // if the node is branch node
    if (node.body_index == -1) {
        // find the octant for the body
        auto octant = getOctant(node.cube.center, body.pos);
        // get the child node index of that octant
        const int32_t child_idx = node.children[static_cast<size_t>(octant)];

        // update mass and center of mass
        node.mass = node.mass + body.mass;
        // update center of mass directly indirectly
        node.cm.v[0] += body.pos.v[0] * body.mass;
        node.cm.v[1] += body.pos.v[1] * body.mass;
        node.cm.v[2] += body.pos.v[2] * body.mass;

        // we do not create all children at once
        // hence it is possible that some children are null
        // if child node does not exist, create it
        if (child_idx == -1) {
            // create child node
            // this doesnt recurse etc
            insertChild(nodes, node, body, body_idx, octant);
        } else {
            // child node exists
            // recursively insert into child node
            insertBody(nodes, bodies, body_idx, child_idx);
        }
    } else {
        // node is a leaf node
        // we need to subdivide the node
        const int32_t body_idx_old = node.body_index;
        // mark as branch node
        node.body_index = -1;

        // first we need to create a child node for the existing body
        // find octant of existing body
        auto octant_old = getOctant(node.cube.center, bodies[body_idx_old].pos);
        // reinsert the existing body
        insertChild(nodes, node, bodies[body_idx_old], body_idx_old,
                    octant_old);

        // then we need to insert the new body
        // this needs to be recursive since both bodies could be in same octant
        insertBody(nodes, bodies, body_idx, node_idx);
    }
}

void finalizeNodes(std::vector<Node> &nodes) {
    for (auto &node : nodes) {
        if (node.mass > 0) {
            node.cm.v[0] /= node.mass;
            node.cm.v[1] /= node.mass;
            node.cm.v[2] /= node.mass;
        }
    }
}

// the force calculation function
// it modifies the body's acceleration directly
// note that theta2 is theta squared
void calculateForceOnBody(const std::vector<Node> &nodes,
                          std::vector<Body> &bodies, int32_t body_idx,
                          int32_t node_idx, double theta2) {
    auto &target_body = bodies[body_idx];
    const auto &node = nodes[node_idx];
    const auto &node_cm = node.cm.v;

    // find position vector respect to body i
    const auto &pos = target_body.pos.v;
    const double dx = node_cm[0] - pos[0];
    const double dy = node_cm[1] - pos[1];
    const double dz = node_cm[2] - pos[2];

    double dist2 = (dx * dx) + (dy * dy) + (dz * dz);

    // if node is leaf node
    if (node.body_index != -1) {
        if (node.body_index == body_idx) {
            // same body, skip
            return;
        }

        // calculate direct force
        dist2 += constants::softening_au;
        const double rdist = 1 / std::sqrt(dist2);
        const double rdist3 = rdist * rdist * rdist;

        // calculate accel magnitude
        const double accel = (constants::g_au * node.mass) * rdist3;

        // update target body's acceleration
        target_body.acc.v[0] += accel * dx;
        target_body.acc.v[1] += accel * dy;
        target_body.acc.v[2] += accel * dz;
        return;
    }

    // size of the node
    const double s = node.cube.half_size * 2.0;
    const double s2 = s * s;

    // Barnes-Hut criterion
    if (s2 < theta2 * dist2) {
        // treat as single body

        // compute softened distance cubed
        dist2 += constants::softening_au;
        const double rdist = 1 / std::sqrt(dist2);
        const double rdist3 = rdist * rdist * rdist;

        // calculate accel magnitude
        const double accel = constants::g_au * node.mass * rdist3;

        // update target body's acceleration
        target_body.acc.v[0] += accel * dx;
        target_body.acc.v[1] += accel * dy;
        target_body.acc.v[2] += accel * dz;
    } else {
        // need to continue traversing children
        for (int i = 0; i < 8; ++i) {
            const int32_t child_idx = node.children[i];
            if (child_idx == -1) {
                continue;
            }
            calculateForceOnBody(nodes, bodies, body_idx, child_idx, theta2);
        }
    }
}

}  // namespace nbodysim