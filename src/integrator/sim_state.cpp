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
void saveSimResultToCSV(const SimResult &results, const std::string &filename) {
    // open file
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // write header
    file << "time, body_id, mass, pos_x, pos_y, pos_z, vel_x, vel_y, vel_z\n";

    // write data
    for (const auto &entry : results.states) {
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

void saveSimResultToMsgPack(const SimResult &states,
                            const std::string &filename) {
    // open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // pack data
    msgpack::pack(file, states);

    file.close();
}

auto loadSimResultFromMsgPack(const std::string &filename) -> SimResult {
    // open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // seek to end to get size
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read file into buffer
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    // unpack data
    msgpack::object_handle oh = msgpack::unpack(buffer.data(), fileSize);
    msgpack::object obj = oh.get();

    // convert to SimResult
    SimResult result;
    obj.convert(result);

    return result;
}

}  // namespace nbodysim