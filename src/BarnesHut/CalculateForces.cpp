#include <cmath>
#include "body.h"
#include "Octan.h"

Vec3 calculateForce(int body_idx, const std::vector<Body>& bodies, Octan* octan, double theta) {

    //initialization of the forces!
    Vec3 force = {0.0f,0.0f,0.0f};

    const Body& body = bodies[body_idx];

    double dx = octan->COM.v[0] - body.pos.v[0];
    double dy = octan->COM.v[1] - body.pos.v[1];
    double dz = octan->COM.v[2] - body.pos.v[2];
    double squaresum = dx*dx + dy*dy + dz*dz;
    double distance = sqrt(squaresum);

    // skip if same body or very close!
    if (squaresum < 1e-10f) return force;

    if (octan->is_leaf && octan->has_body) {
        if (octan->body_index == body_idx) {
            return force;
        }
        double F = G * body.mass * octan->body_mass/squaresum;
        force.v[0] = F*dx/distance;
        force.v[1] = F*dy/distance;
        force.v[2] = F*dz/distance;
        return force;
    }

    // if the leaf is empty!
    if (octan->is_leaf && !octan->has_body) {
        return force;
    }

    // width of the octant!
    double s = octan->xmax - octan->xmin;
    // distance to the center of mass!
    double d = distance;

    if (s/d < theta) {
        double F = G * body.mass * octan->body_mass/squaresum;
        force.v[0] = F*dx/distance;
        force.v[1] = F*dy/distance;
        force.v[2] = F*dz/distance;
        return force;
    }

    else {
        for (int i = 0; i<8; i++) {
            if (octan->children[i] != nullptr) {
                Vec3 childForce = calculateForce(body_idx, bodies, octan->children[i], theta);
                force.v[0] += childForce.v[0];
                force.v[1] += childForce.v[1];
                force.v[2] += childForce.v[2];
            }
        }
    }

    return force;
}













