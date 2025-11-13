//
// Created by Doruk Gürkan on 12.11.2025.
//

#ifndef NBODY_BODY_H
#define NBODY_BODY_H
#include <cmath>

struct Vec3 {
	double_t v[3];
};

struct Body {
    uint32_t id;
    double_t mass;
    Vec3 pos;
    Vec3 vel;
};

#endif  // NBODY_BODY_H
