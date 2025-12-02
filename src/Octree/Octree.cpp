#include <iostream>
#include <vector>
#include "body.h"
#include "Octan.h"


void insert_body(Octan& oct, const std::vector<Body>& bodies, int body_idx, int& max_depth) {
    const Body& body = bodies[body_idx];
    max_depth = std::max(max_depth, oct.depth);

    // CASE 1: Empty leaf - simple insertion
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

    // CASE 2: Leaf with a body - need to subdivide
    if (oct.is_leaf && oct.has_body) {
        // CRITICAL: Store existing body BEFORE subdivision
        int existing_body_idx = oct.body_index;
        const Body& existing_body = bodies[existing_body_idx];

        // Subdivide
        recursively_subdivide(oct);
        max_depth = std::max(max_depth, oct.depth);

        // CRITICAL: Clear parent's body info (it's now internal)
        oct.has_body = false;
        oct.body_index = -1;

        // Find where each body goes
        int existing_octant = get_octant(oct, existing_body);
        int new_octant = get_octant(oct, body);

        // Recursively insert BOTH bodies
        insert_body(*oct.children[existing_octant], bodies, existing_body_idx, max_depth);
        insert_body(*oct.children[new_octant], bodies, body_idx, max_depth);

        return;
    }

    // CASE 3: Internal node - route to child
    if (!oct.is_leaf) {
        int octant = get_octant(oct, body);
        insert_body(*oct.children[octant], bodies, body_idx, max_depth);
    }
}

Octan* buildOctree(std::vector<Body>& bodies) {

    int spacetreedepth = 0;

    double xmin = bodies[0].pos.v[0];
    double xmax = bodies[0].pos.v[0];
    double ymin = bodies[0].pos.v[1];
    double ymax = bodies[0].pos.v[1];
    double zmin = bodies[0].pos.v[2];
    double zmax = bodies[0].pos.v[2];

    for (size_t i = 1; i < bodies.size(); i++) {
        if (bodies[i].pos.v[0] < xmin) xmin = bodies[i].pos.v[0];
        if (bodies[i].pos.v[0] > xmax) xmax = bodies[i].pos.v[0];
        if (bodies[i].pos.v[1] < ymin) ymin = bodies[i].pos.v[1];
        if (bodies[i].pos.v[1] > ymax) ymax = bodies[i].pos.v[1];
        if (bodies[i].pos.v[2] < zmin) zmin = bodies[i].pos.v[2];
        if (bodies[i].pos.v[2] > zmax) zmax = bodies[i].pos.v[2];
    }

    // add small padding!
    double range = std::max({xmax - xmin, ymax - ymin, zmax - zmin});
    double padding = 0.01 * range;
    xmin -= padding;
    xmax += padding;
    ymin -= padding;
    ymax += padding;
    zmin -= padding;
    zmax += padding;

    // create root node!
    Octan* root = new Octan();
    root->xmin = xmin;
    root->xmax = xmax;
    root->ymin = ymin;
    root->ymax = ymax;
    root->zmin = zmin;
    root->zmax = zmax;

    root->is_leaf = true;

    // Insert the bodies into the octree
    for (int i = 0; i < static_cast<int>(bodies.size()); ++i) {
        insert_body(*root, bodies, i, spacetreedepth);
    }

    //std::cout << "Successfully inserted " << bodies.size() << " bodies\n";
    //std::cout << "Maximum tree depth: " << spacetreedepth << "\n";
    //print_tree_structure(*root, 0, -1);

    return root;
}