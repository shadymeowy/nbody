#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "common/body.hpp"
#include "common/vec.hpp"
#include "space/octree.hpp"

namespace nb = nbodysim;

// this is a utility function to check octree integrity
// throws if integrity check fails
// thanks to linear storage of nodes, we can easily traverse the tree
// it performs following checks:
//      1. total number of bodies matches
//      2. total mass and center of mass at root node
//      3. each branch node's mass and center of mass matches sum of its
//      children
//      4. each node (except root) is referenced exactly once by its
//      parent
//      5. each body is referenced exactly once by its associated node
//      6. each body shares same weights and positions as their assigned
//      node
//      7. each body is within their node cubes
//      8. each child is proper octant of its parent

static void checkOctreeIntegrity(const std::vector<nb::Node> &nodes,
                                 const std::vector<nb::Body> &bodies) {
    // first lets define atol and rtol
    const double atol = 1e-9;
    const double rtol = 1e-9;

    // helper lambda for comparing doubles
    auto isclose = [atol, rtol](double a, double b) -> bool {
        // ref: good old numpy's isclose
        // https://numpy.org/doc/stable/reference/generated/numpy.isclose.html
        return std::abs(a - b) <= (atol + (rtol * std::abs(b)));
    };
    auto isclose3 = [isclose](const nb::Vec3 &a, const nb::Vec3 &b) -> bool {
        return isclose(a.v[0], b.v[0]) && isclose(a.v[1], b.v[1]) &&
               isclose(a.v[2], b.v[2]);
    };

    // 1. total number of bodies matches
    size_t body_count = 0;
    for (const auto &node : nodes) {
        if (node.body_index != -1) {
            body_count++;
        }
    }
    if (body_count != bodies.size()) {
        throw std::runtime_error(
            "Octree integrity check failed: body count mismatch.");
    }

    // 2. total mass and center of mass at root node
    const nb::Node &root = nodes[0];
    double total_mass = 0.0;
    nb::Vec3 com{0.0, 0.0, 0.0};
    for (const auto &body : bodies) {
        total_mass += body.mass;
        for (int i = 0; i < 3; ++i) {
            com.v[i] += body.pos.v[i] * body.mass;
        }
    }
    for (int i = 0; i < 3; ++i) {
        com.v[i] /= total_mass;
    }
    if (!isclose(root.mass, total_mass)) {
        throw std::runtime_error(
            "Octree integrity check failed: total mass mismatch.");
    }
    if (!isclose3(root.cm, com)) {
        throw std::runtime_error(
            "Octree integrity check failed: center of mass mismatch.");
    }

    // 3. each branch node's mass and center of mass matches sum of its
    // children
    for (const auto &node : nodes) {
        if (node.body_index == -1) {
            // branch node
            double branch_mass = 0.0;
            nb::Vec3 branch_com{0.0, 0.0, 0.0};
            for (int i = 0; i < 8; ++i) {
                const int32_t child_idx = node.children[i];
                if (child_idx == -1) {
                    continue;
                }
                const nb::Node &child = nodes[child_idx];
                branch_mass += child.mass;
                for (int j = 0; j < 3; ++j) {
                    branch_com.v[j] += child.cm.v[j] * child.mass;
                }
            }
            for (int j = 0; j < 3; ++j) {
                branch_com.v[j] /= branch_mass;
            }
            if (!isclose(node.mass, branch_mass)) {
                throw std::runtime_error(
                    "Octree integrity check failed: branch node mass "
                    "mismatch.");
            }
            if (!isclose3(node.cm, branch_com)) {
                throw std::runtime_error(
                    "Octree integrity check failed: branch node center of "
                    "mass mismatch.");
            }
        }
    }

    // 4. each node (except root) is referenced exactly once by its parent
    // first create an array to track references
    std::vector<bool> referenced(nodes.size(), false);
    // root node is not referenced by any parent
    // hence mark manually
    referenced[0] = true;
    // and mark all children as referenced
    // if any node is referenced more than once, integrity check fails
    for (const auto &node : nodes) {
        for (int i = 0; i < 8; ++i) {
            const int32_t child_idx = node.children[i];
            if (child_idx == -1) {
                continue;
            }
            if (referenced[child_idx]) {
                throw std::runtime_error(
                    "Octree integrity check failed: node referenced more "
                    "than "
                    "once.");
            }
            referenced[child_idx] = true;
        }
    }

    // check that all nodes are referenced
    // using another pass
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!referenced[i]) {
            throw std::runtime_error(
                "Octree integrity check failed: unreferenced node found.");
        }
    }

    // 5. each body is referenced exactly once by its associated node
    // ie all bodies are referenced exactly once
    std::vector<bool> body_referenced(bodies.size(), false);
    for (const auto &node : nodes) {
        if (node.body_index == -1) {
            continue;
        }
        const int32_t bidx = node.body_index;
        if (body_referenced[bidx]) {
            throw std::runtime_error(
                "Octree integrity check failed: body referenced more than "
                "once.");
        }
        body_referenced[bidx] = true;
    }
    for (size_t i = 0; i < bodies.size(); ++i) {
        if (!body_referenced[i]) {
            throw std::runtime_error(
                "Octree integrity check failed: unreferenced body found.");
        }
    }

    // 6. each body shares same weights and positions as their assigned node
    for (const auto &node : nodes) {
        if (node.body_index == -1) {
            continue;
        }
        const nb::Body &body = bodies[node.body_index];
        if (!isclose(body.mass, node.mass)) {
            throw std::runtime_error(
                "Octree integrity check failed: body mass mismatch.");
        }
        if (!isclose3(body.pos, node.cm)) {
            throw std::runtime_error(
                "Octree integrity check failed: body position mismatch.");
        }
    }

    // 7. each body is within their node cubes
    for (const auto &node : nodes) {
        if (node.body_index == -1) {
            continue;
        }
        const nb::Body &body = bodies[node.body_index];
        for (int i = 0; i < 3; ++i) {
            if (body.pos.v[i] < node.cube.center.v[i] - node.cube.half_size ||
                body.pos.v[i] > node.cube.center.v[i] + node.cube.half_size) {
                throw std::runtime_error(
                    "Octree integrity check failed: body position out of "
                    "node "
                    "bounds.");
            }
        }
    }

    // 8. all children are proper octants of their parents
    for (const auto &node : nodes) {
        const nb::BCube &cube = node.cube;

        for (uint32_t i = 0; i < 8; ++i) {
            const int32_t child_idx = node.children[i];

            if (child_idx == -1) {
                continue;
            }

            const nb::BCube &child_cube = nodes[child_idx].cube;
            // using child center to determine octant
            auto octant = nb::getOctant(cube.center, child_cube.center);
            if (octant != i) {
                throw std::runtime_error(
                    "Octree integrity check failed: child octant "
                    "mismatch.");
            }
            // then use child's center and size to get center of parent
            const double half_size = cube.half_size * 0.5;
            const nb::Vec3 expected_center{
                ((i & 1U) != 0) ? (cube.center.v[0] + half_size)
                                : (cube.center.v[0] - half_size),
                ((i & 2U) != 0) ? (cube.center.v[1] + half_size)
                                : (cube.center.v[1] - half_size),
                ((i & 4U) != 0) ? (cube.center.v[2] + half_size)
                                : (cube.center.v[2] - half_size),
            };
            if (!isclose3(child_cube.center, expected_center)) {
                throw std::runtime_error(
                    "Octree integrity check failed: child cube center "
                    "mismatch.");
            }
            // check child's half size
            if (!isclose(child_cube.half_size, half_size)) {
                throw std::runtime_error(
                    "Octree integrity check failed: child cube half size "
                    "mismatch.");
            }
        }
    }
}