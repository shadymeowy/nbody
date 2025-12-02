#include "Octan.h"
#include "body.h"

int get_octant(const Octan& oct, const Body& body) {
    double xmid = 0.5 * (oct.xmin + oct.xmax);
    double ymid = 0.5 * (oct.ymin + oct.ymax);
    double zmid = 0.5 * (oct.zmin + oct.zmax);

    int index = 0;
    if (body.pos.v[0] >= xmid) index |= 4;
    if (body.pos.v[1] >= ymid) index |= 2;
    if (body.pos.v[2] >= zmid) index |= 1;
    return index;
}

