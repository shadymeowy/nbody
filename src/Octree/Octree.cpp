#include <iostream>
#include <ranges>
#include <vector>
#include "body.h"
#include "Octan.h"


void insert_body(Octan& oct, const std::vector<Body>& bodies, int body_idx, int& max_depth) {
    const Body& body = bodies[body_idx];
    max_depth = std::max(max_depth, oct.depth);

    // if Empty leaf, insert the body!
    if (oct.is_leaf && !oct.has_body) {
        oct.has_body = true;
        oct.body_index = body_idx;
        oct.body = body;
        oct.COM.v[0] = body.pos.v[0];
        oct.COM.v[1] = body.pos.v[1];
        oct.COM.v[2] = body.pos.v[2];
        oct.body_mass=body.mass;
        return;
    }

    // if Leaf has a body, we need to subdivide it to be ensure that each octan contains only 1 body!
    if (oct.is_leaf && oct.has_body) {
        // Storing body avaliable in octree before recursive subdivision!
        int existing_body_idx = oct.body_index;
        const Body& existing_body = bodies[existing_body_idx];

        // Subdivide recursively!
        recursively_subdivide(oct);
        max_depth = std::max(max_depth, oct.depth);

        // Clear parent's information!
        oct.has_body = false;
        oct.body_index = -1;

        // Find where each body will be present in octree!
        int existing_octant = get_octant(oct, existing_body);
        int new_octant = get_octant(oct, body);

        // Recursively insert both bodies into octree!
        insert_body(*oct.children[existing_octant], bodies, existing_body_idx, max_depth);
        insert_body(*oct.children[new_octant], bodies, body_idx, max_depth);

        return;
    }

    // Internal Nodes/Octants!
    if (!oct.is_leaf) {
        int octant = get_octant(oct, body);
        insert_body(*oct.children[octant], bodies, body_idx, max_depth);
    }
}

Octan* buildOctree(std::vector<Body>& bodies) {

    // this is spacetree depth, this should not be too large!
    int spacetreedepth = 0;

    // the bounding box of the octree!
    double xmin = bodies[0].pos.v[0];
    double xmax = bodies[0].pos.v[0];
    double ymin = bodies[0].pos.v[1];
    double ymax = bodies[0].pos.v[1];
    double zmin = bodies[0].pos.v[2];
    double zmax = bodies[0].pos.v[2];

    // the bounding box is updated followingly to get an dynamic structure
    // for handling the changes in the data of the bodies!
    for (size_t i = 1; i < bodies.size(); i++) {
        if (bodies[i].pos.v[0] < xmin) xmin = bodies[i].pos.v[0];
        if (bodies[i].pos.v[0] > xmax) xmax = bodies[i].pos.v[0];
        if (bodies[i].pos.v[1] < ymin) ymin = bodies[i].pos.v[1];
        if (bodies[i].pos.v[1] > ymax) ymax = bodies[i].pos.v[1];
        if (bodies[i].pos.v[2] < zmin) zmin = bodies[i].pos.v[2];
        if (bodies[i].pos.v[2] > zmax) zmax = bodies[i].pos.v[2];
    }

    // adding small allowance to prevent that the points are at the edge of the octree!
    double range = std::max({xmax - xmin, ymax - ymin, zmax - zmin});
    double allowance = 0.01 * range;
    xmin -= allowance;
    xmax += allowance;
    ymin -= allowance;
    ymax += allowance;
    zmin -= allowance;
    zmax += allowance;

    // creating root octant!
    Octan* root = new Octan();
    root->xmin = xmin;
    root->xmax = xmax;
    root->ymin = ymin;
    root->ymax = ymax;
    root->zmin = zmin;
    root->zmax = zmax;

    root->is_leaf = true;

    // Inserting the bodies into the octree!
    for (int i = 0; i < static_cast<int>(bodies.size()); ++i) {
        insert_body(*root, bodies, i, spacetreedepth);
    }

    return root;
}