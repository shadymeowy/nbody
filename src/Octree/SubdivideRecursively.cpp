#include "Octan.h"

void recursively_subdivide(Octan& oct) {
    double xmid = 0.5 * (oct.xmin + oct.xmax);
    double ymid = 0.5 * (oct.ymin + oct.ymax);
    double zmid = 0.5 * (oct.zmin + oct.zmax);

    // Octant 0: (-x, -y, -z)
    Octan* oct0 = new Octan();
    oct0->xmin = oct.xmin;
    oct0->xmax = xmid;
    oct0->ymin = oct.ymin;
    oct0->ymax = ymid;
    oct0->zmin = oct.zmin;
    oct0->zmax = zmid;
    init_leaf_octants(*oct0);

    // Octant 1: (-x, -y, +z)
    Octan* oct1 = new Octan();
    oct1->xmin = oct.xmin;
    oct1->xmax = xmid;
    oct1->ymin = oct.ymin;
    oct1->ymax = ymid;
    oct1->zmin = zmid;
    oct1->zmax = oct.zmax;
    init_leaf_octants(*oct1);

    // Octant 2: (-x, +y, -z)
    Octan* oct2 = new Octan();
    oct2->xmin = oct.xmin;
    oct2->xmax = xmid;
    oct2->ymin = ymid;
    oct2->ymax = oct.ymax;
    oct2->zmin = oct.zmin;
    oct2->zmax = zmid;
    init_leaf_octants(*oct2);

    // Octant 3: (-x, +y, +z)
    Octan* oct3 = new Octan();
    oct3->xmin = oct.xmin;
    oct3->xmax = xmid;
    oct3->ymin = ymid;
    oct3->ymax = oct.ymax;
    oct3->zmin = zmid;
    oct3->zmax = oct.zmax;
    init_leaf_octants(*oct3);

    // Octant 4: (+x, -y, -z)
    Octan* oct4 = new Octan();
    oct4->xmin = xmid;
    oct4->xmax = oct.xmax;
    oct4->ymin = oct.ymin;
    oct4->ymax = ymid;
    oct4->zmin = oct.zmin;
    oct4->zmax = zmid;
    init_leaf_octants(*oct4);

    // Octant 5: (+x, -y, +z)
    Octan* oct5 = new Octan();
    oct5->xmin = xmid;
    oct5->xmax = oct.xmax;
    oct5->ymin = oct.ymin;
    oct5->ymax = ymid;
    oct5->zmin = zmid;
    oct5->zmax = oct.zmax;
    init_leaf_octants(*oct5);

    // Octant 6: (+x, +y, -z)
    Octan* oct6 = new Octan();
    oct6->xmin = xmid;
    oct6->xmax = oct.xmax;
    oct6->ymin = ymid;
    oct6->ymax = oct.ymax;
    oct6->zmin = oct.zmin;
    oct6->zmax = zmid;
    init_leaf_octants(*oct6);

    // Octant 7: (+x, +y, +z)
    Octan* oct7 = new Octan();
    oct7->xmin = xmid;
    oct7->xmax = oct.xmax;
    oct7->ymin = ymid;
    oct7->ymax = oct.ymax;
    oct7->zmin = zmid;
    oct7->zmax = oct.zmax;
    init_leaf_octants(*oct7);

    // Assign children
    oct.children[0] = oct0;
    oct.children[1] = oct1;
    oct.children[2] = oct2;
    oct.children[3] = oct3;
    oct.children[4] = oct4;
    oct.children[5] = oct5;
    oct.children[6] = oct6;
    oct.children[7] = oct7;

    for (int i = 0; i < 8; i++) {
        oct.children[i]->depth = oct.depth + 1;
    }

    oct.is_leaf = false;
}


