#include "body.h"
#include "Octan.h"
#include <cstdlib>

void centerOfMass(Octan* octan) {

    if (octan->is_leaf) {
        // Handling empty leaf!
        if (!octan->has_body) {
            octan->body_mass = 0.0f;
            octan->COM.v[0] = 0.0f;
            octan->COM.v[1] = 0.0f;
            octan->COM.v[2] = 0.0f;
            return;
        }

        // Leaf octant with body!
        // if it contains just one body, we are going to initialize it as exactly the bodies
        // mass and position
        octan->body_mass = octan->body.mass;
        octan->COM.v[0] = octan->body.pos.v[0];
        octan->COM.v[1] = octan->body.pos.v[1];
        octan->COM.v[2] = octan->body.pos.v[2];
        return;
    }

    //initialization!!
    //octan->body_mass = 0.0f;
    //octan->COM.v[0] = 0.0f;
    //octan->COM.v[1] = 0.0f;
    //octan->COM.v[2] = 0.0f;
    // if it contains more than one point, what we are going to do will be recursively calculate the com and total mass
    // for each children octant!
    for (int i=0; i<8; i++) {
        if (octan->children[i] != nullptr) {
            centerOfMass(octan->children[i]);

            double childMass = octan->children[i]->body_mass;
            Vec3 childCOM = octan->children[i]->COM;
            octan->COM.v[0] += childMass*childCOM.v[0];
            octan->COM.v[1] += childMass*childCOM.v[1];
            octan->COM.v[2] += childMass*childCOM.v[2];
            octan->body_mass += childMass;
        }
    }
    // what we are doing is, we actually calculated the mass*COM for each of the octant's regardless of whether
    // it contains a body or not!, so if it contains body what we are doing will be finalizing the com calculation
    // by dividing with the total mass!
    if (octan->body_mass > 0.0f) {
        octan->COM.v[0] /= octan->body_mass;
        octan->COM.v[1] /= octan->body_mass;
        octan->COM.v[2] /= octan->body_mass;
    }

}






