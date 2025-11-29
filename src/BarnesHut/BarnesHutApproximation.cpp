#include "Octan.h"
#include "body.h"
#include <fstream>
extern Vec3 calculateForce(int body_idx, const std::vector<Body>& bodies, Octan* octan, double theta);
extern Octan* buildOctree(std::vector<Body>& bodies);

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
    double dx = b->pos.v[0] - a->pos.v[0];
    double dy = b->pos.v[1] - a->pos.v[1];
    double dz = b->pos.v[2] - a->pos.v[2];
    double distance = sqrt(dx*dx + dy*dy + dz*dz);

    // avoid division by zero!
    if (distance < 1e-10) {
        distance = 1e-10;
        dx = 1.0;
        dy = 0.0;
        dz = 0.0;
    }

    double radiusSum = a->radius + b->radius;

    if (distance >= radiusSum) return;
    //std::cout << "COLLISION: Body " << a->id << " and Body " << b->id << "\n";

    // normalize the directions!
    double nx = dx / distance;
    double ny = dy / distance;
    double nz = dz / distance;

    // rectraction of the bodies!
    double overlap = radiusSum - distance;
    double totalMass = a->mass + b->mass;
    double ratioA = b->mass/totalMass;
    double ratioB = a->mass/totalMass;

    a->pos.v[0] -= nx * overlap * ratioA;
    a->pos.v[1] -= ny * overlap * ratioA;
    a->pos.v[2] -= nz * overlap * ratioA;

    b->pos.v[0] += nx * overlap * ratioB;
    b->pos.v[1] += ny * overlap * ratioB;
    b->pos.v[2] += nz * overlap * ratioB;

    // Adjust velocities!
    double dvx = a->vel.v[0] - b->vel.v[0];
    double dvy = a->vel.v[1] - b->vel.v[1];
    double dvz = a->vel.v[2] - b->vel.v[2];


    // Relative velocity along collision normal!
    double dvn = dvx * nx + dvy * ny + dvz * nz;

    // Only resolve if bodies are approaching
    if (dvn > 0) return;

    // Impulse force in scalar!
    double impulse = (2.0 * dvn) / totalMass;

    // Apply impulse to both bodies!
    a->vel.v[0] -= impulse * b->mass * nx;
    a->vel.v[1] -= impulse * b->mass * ny;
    a->vel.v[2] -= impulse * b->mass * nz;

    b->vel.v[0] += impulse * a->mass * nx;
    b->vel.v[1] += impulse * a->mass * ny;
    b->vel.v[2] += impulse * a->mass * nz;
}

bool checkCollision(Body* a, Body* b) {
    double dx = a->pos.v[0] - b->pos.v[0];
    double dy = a->pos.v[1] - b->pos.v[1];
    double dz = a->pos.v[2] - b->pos.v[2];
    double distance = sqrt(dx*dx + dy*dy + dz*dz);
    double distance_sum = a->radius + b->radius;
    return distance < distance_sum*distance_sum;
}


void detectCollisionsOctree(int body_idx, Body* body, Octan* octan) {
    double dx = octan->COM.v[0] - body->pos.v[0];
    double dy = octan->COM.v[1] - body->pos.v[1];
    double dz = octan->COM.v[2] - body->pos.v[2];
    double distance = sqrt(dx*dx + dy*dy + dz*dz);

    // Maximum possible collision distance!
    double octantSize = octan->xmax - octan->xmin;
    double maxDist = (octan->xmax-octan->xmin) * 1.732 + body->radius;
    if (distance > maxDist) {
        return;
    }

    if (octan->is_leaf) {
        if (!octan->has_body) {
            return;
        }
        //std::cout << "Checking: body_idx=" << body_idx
        //      << " vs octan->body_index=" << octan->body_index
        //      << " has_body=" << octan->has_body << "\n";
        if (octan->body_index == body_idx) {
            return;
        }
        if (body_idx > octan->body_index) return;

        if (octan->body_index < 0) {
            return;
        }

        if (checkCollision(body, &octan->body)) {
            //std::cout << "COLLISION: Body " << body_idx
            //                      << " and Body " << octan->body_index << "\n";
            handleCollision(body, &octan->body);
        }
        return;
    }
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

        // update position!
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
        // Write current state to CSV FIRST (before update for step 0)
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
        // Free memory
        DeleteOctree(root);
    }

    file.close();
    std::cout << "Saved simulation to simulation.csv\n";
}