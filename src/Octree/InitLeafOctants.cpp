#include "Octan.h"

// in this function, the newly formed octant's properties, from recursive subdivision, has been initialized!
void init_leaf_octants(Octan& oct) {
    oct.is_leaf = true;
    oct.has_body = false;
    oct.body_index = -1;
    oct.body = Body{};
    for (int q=0; q<8; ++q) {
        oct.children[q] = nullptr;
    }
    oct.depth = 0;
    oct.COM.v[0] = 0.5 * (oct.xmin + oct.xmax);
    oct.COM.v[1] = 0.5 * (oct.ymin + oct.ymax);
    oct.COM.v[2] = 0.5 * (oct.zmin + oct.zmax);
    oct.body_mass = 0;
}