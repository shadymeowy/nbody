#include "Octan.h"
#include "body.h"
#include <fstream>

void calculateAllForce(std::vector<Body>& bodies, Octan* octan, double theta) {
    for (int i = 0; i < bodies.size(); i++) {
        // initialize the forces as 0 for each step!
        bodies[i].force.v[0] = 0.0f;
        bodies[i].force.v[1] = 0.0f;
        bodies[i].force.v[2] = 0.0f;

        bodies[i].force = calculateForce(i, bodies, octan, theta);
    }
}

void handleCollision(Body* a, Body* b) {

    // elastic collision with using linear impulse method!
    // on each body in case of collision!
    double dx = b->pos.v[0] - a->pos.v[0];
    double dy = b->pos.v[1] - a->pos.v[1];
    double dz = b->pos.v[2] - a->pos.v[2];
    double distance = sqrt(dx*dx + dy*dy + dz*dz);

    // avoid division by zero! (debugging!)
    if (distance < 1e-10) {
        distance = 1e-10;
        dx = 1.0;
        dy = 0.0;
        dz = 0.0;
    }

    double radiusSum = a->radius + b->radius;
    // we are checking again to see whether overlap exists (debugging!)
    if (distance*distance >= radiusSum*radiusSum) return;
    //std::cout << "COLLISION: Body " << a->id << " and Body " << b->id << "\n";

    // normalize the directions for each component!
    // the calculation will be made for each component!
    double nx = dx / distance;
    double ny = dy / distance;
    double nz = dz / distance;

    // we took C_R, e = 1, which is coefficient of restitution!, for an elastic case!!
    // retraction of the bodies!
    double overlap = radiusSum - distance;
    double totalMass = a->mass + b->mass;
    // depends upon the fraction, which point will react more will be obtained!!
    double ratioA = b->mass/totalMass;
    double ratioB = a->mass/totalMass;

    a->pos.v[0] -= nx * overlap * ratioA;
    a->pos.v[1] -= ny * overlap * ratioA;
    a->pos.v[2] -= nz * overlap * ratioA;

    b->pos.v[0] += nx * overlap * ratioB;
    b->pos.v[1] += ny * overlap * ratioB;
    b->pos.v[2] += nz * overlap * ratioB;

    // Adjusting velocities!
    double dvx = a->vel.v[0] - b->vel.v[0];
    double dvy = a->vel.v[1] - b->vel.v[1];
    double dvz = a->vel.v[2] - b->vel.v[2];

    // Relative velocity along collision normal!
    double dvn = dvx * nx + dvy * ny + dvz * nz;

    // Only resolve if bodies are approaching
    // being vr.n negative means the bodies are separating it from each other, so no need to make calculation!
    if (dvn < 0) return;

    // Impulse force in scalar!
    double impulse = -(2.0 * dvn) / totalMass;

    // Apply impulse to both bodies!
    a->vel.v[0] += impulse * b->mass * nx;
    a->vel.v[1] += impulse * b->mass * ny;
    a->vel.v[2] += impulse * b->mass * nz;

    b->vel.v[0] -= impulse * a->mass * nx;
    b->vel.v[1] -= impulse * a->mass * ny;
    b->vel.v[2] -= impulse * a->mass * nz;
}

bool checkCollision(Body* a, Body* b) {
    // if in bounding spheres of the bodies this condition satisfied:
    // dist < R1 + R2, return true; otherwise, false!
    double dx = a->pos.v[0] - b->pos.v[0];
    double dy = a->pos.v[1] - b->pos.v[1];
    double dz = a->pos.v[2] - b->pos.v[2];
    double distance = dx*dx + dy*dy + dz*dz;
    double distance_sum = a->radius + b->radius;
    // avoiding square root: (x1-x2)^2 + (y1-y2)^2 < (R1+R2)^2!
    return distance < distance_sum*distance_sum;
}


void detectCollisionsOctree(int body_idx, Body* body, Octan* octan) {
    double dx = octan->COM.v[0] - body->pos.v[0];
    double dy = octan->COM.v[1] - body->pos.v[1];
    double dz = octan->COM.v[2] - body->pos.v[2];
    double distance = sqrt(dx*dx + dy*dy + dz*dz);

    // Maximum possible collision distance!
    double octantSize = octan->xmax - octan->xmin;
    // we are calculating the maximum distance to see whether they are too far away from each other for collision!
    // the variable 1.732 is actually about sqrt(3) which is the space diagonal of the unit cube, taken as like this for efficiency!
    // distance from a
    // Center of Mass (COM) to the farthest point in octant = octant size * sqrt(3)/2
    // and adding the body fictitous contact radius!
    double maxDist = octantSize * 1.732/2 + body->radius;

    // if they are very far from each other, no need to make detection!
    if (distance > maxDist) {
        return;
    }

    // if the octan does not have body; then no need to check for interaction!
    if (octan->is_leaf) {
        if (!octan->has_body) {
            return;
        }

        //std::cout << "Checking: body_idx=" << body_idx
        //      << " vs octan->body_index=" << octan->body_index
        //      << " has_body=" << octan->has_body << "\n";
        // avoid checking the same body again! (debugging!)
        if (octan->body_index == body_idx) {
            return;
        }
        // if the octan's contained body does not match with the respective body (debugging),
        // since the bodies are entering randomly might happen!
        if (body_idx > octan->body_index) return;
        // invalid index check!
        if (octan->body_index < 0) {
            return;
        }
        // if 3-d collision check returns true handle the collision!!
        if (checkCollision(body, &octan->body)) {
            //std::cout << "COLLISION: Body " << body_idx
            //                      << " and Body " << octan->body_index << "\n";
            handleCollision(body, &octan->body);
        }
        return;
    }
    // recursively check, whether all the children of octan has a body!!
    for (int i=0; i<8; i++) {
        if (octan->children[i] != nullptr) {
            detectCollisionsOctree(body_idx, body, octan->children[i]);
        }
    }
}

void detectAllCollisions(std::vector<Body>& bodies, Octan* root) {
    for (int i=0; i<bodies.size(); i++) {
        detectCollisionsOctree(i, &bodies[i], root);
    }
}

void updateBodies(std::vector<Body>& bodies, double dt) {
    for (int i=0; i<bodies.size(); i++) {
        // acceleration of all components of the corresponding body!
        double ax = bodies[i].force.v[0] / bodies[i].mass;
        double ay = bodies[i].force.v[1] / bodies[i].mass;
        double az = bodies[i].force.v[2] / bodies[i].mass;

        // updating velocity!
        bodies[i].vel.v[0] += ax * dt;
        bodies[i].vel.v[1] += ay * dt;
        bodies[i].vel.v[2] += az * dt;

        // updating positions!
        bodies[i].pos.v[0] += bodies[i].vel.v[0] * dt;
        bodies[i].pos.v[1] += bodies[i].vel.v[1] * dt;
        bodies[i].pos.v[2] += bodies[i].vel.v[2] * dt;

    }
}

void DeleteOctree(Octan* octan) {

    // first free all the children!
    for (int i=0; i<8; i++) {
        if (octan->children[i] != nullptr) {
            DeleteOctree(octan->children[i]);
            octan->children[i] = nullptr;
        }
    }
    // free the memory of the current octant!
    delete octan;
}

void BarnesHutApproximation(std::vector<Body>& bodies, double dt, double theta, int steps) {
    std::ofstream file("/Users/edizhanyildiz/CLionProjects/25/src/simulation.csv");
    file << "step,body_id,x,y,z,vx,vy,vz,mass,radius\n";

    for (int step = 0; step < steps; step++) {
        // Writing only the current state!
        for (size_t i = 0; i < bodies.size(); i++) {
            file << step << ","
                 << bodies[i].id << ","
                 << bodies[i].pos.v[0] << ","
                 << bodies[i].pos.v[1] << ","
                 << bodies[i].pos.v[2] << ","
                 << bodies[i].vel.v[0] << ","
                 << bodies[i].vel.v[1] << ","
                 << bodies[i].vel.v[2] << ","
                 << bodies[i].mass << ","
                 << bodies[i].radius << "\n";
        }

        // Build octree
        Octan* root = buildOctree(bodies);
        // Compute center of mass
        centerOfMass(root);
        // Calculate forces for all bodies
        calculateAllForce(bodies, root, theta);
        // Detect collisions
        detectAllCollisions(bodies, root);
        // Update velocity and positions
        updateBodies(bodies, dt);
        // Free memory of the octree for the current step which will be invalid at that time!
        DeleteOctree(root);
    }

    file.close();
    std::cout << "Saved simulation to simulation.csv\n";
}