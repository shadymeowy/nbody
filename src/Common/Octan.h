#include "body.h"
#include <iostream>
#ifndef NBODY_OCTAN_H
#define NBODY_OCTAN_H


struct Octan {

    // Bounding box!
    double xmin, xmax;
    double ymin, ymax;
    double zmin, zmax;
    // Center of Mass has been initialized!
    Vec3 COM;
    // the spacetree depth of the corresponding octant!
    int depth;
    // this needs to be revised!
    std::array<Octan*, 8> children = {};
    Body body;
    int body_index;
    bool is_leaf;
    bool has_body;
    std::vector<int> Body;
    double body_mass;

};
//void print_tree_structure(const Octan& node, int indent = 0, int octant_index = -1,
//                          int* out_max_depth = nullptr, std::vector<int>* out_level_counts = nullptr);
int getOctant(const Octan& oct, const Body& body);
void init_leaf_octants(Octan& oct);
void recursively_subdivide(Octan& oct);
void centerOfMass(Octan* octan);
Vec3 calculateForce(int body_idx, const std::vector<Body>& bodies, Octan* octan, double theta);
Octan* buildOctree(std::vector<Body>& bodies);

#endif //NBODY_OCTAN_H