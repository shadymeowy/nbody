#include "sim_state.hpp"

#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// see sim_state.hpp for details

namespace nbodysim {

// save to csv
// format:
// time, body_id, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z
void saveSimStateToCSV(const std::vector<SimState> &states,
                       const std::string &filename) {
    // open file
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // write header
    file << "time, body_id, mass, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z\n";

    // write data
    for (const auto &entry : states) {
        const double time = entry.time;
        const auto &bodies = entry.bodies;
        for (size_t i = 0; i < bodies.size(); i++) {
            const auto &body = bodies[i];
            file << time << ", " << i << ", " << body.mass << ", "
                 << body.pos.v[0] << ", " << body.pos.v[1] << ", "
                 << body.pos.v[2] << ", " << body.vel.v[0] << ", "
                 << body.vel.v[1] << ", " << body.vel.v[2] << "\n";
        }
    }
}

}  // namespace nbodysim