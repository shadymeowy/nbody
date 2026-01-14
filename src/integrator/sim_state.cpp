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
void SimResult::saveToCSV(const std::string &filename) const {
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

// load from csv
auto SimResult::loadFromCSV(const std::string &filename) -> SimResult {
    // open file
    std::ifstream file(filename);

    // check if file opened successfully
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    // skip header
    std::getline(file, line);

    // allocate states
    std::vector<SimState> states;
    double current_time = 0.0;
    std::vector<Body> bodies;

    // read file line by line
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        Body body;
        double time;
        size_t body_id;
        char comma;

        // parse line using istringstream
        ss >> time >> comma >> body_id >> comma >> body.mass >> comma >>
            body.pos.v[0] >> comma >> body.pos.v[1] >> comma >> body.pos.v[2] >>
            comma >> body.vel.v[0] >> comma >> body.vel.v[1] >> comma >>
            body.vel.v[2];

        // if time changes, this means we need to start a new SimState
        if (time != current_time) {
            if (!bodies.empty()) {
                states.emplace_back(current_time, bodies);
                bodies.clear();
            }
            current_time = time;
        }

        // add body to current list
        bodies.push_back(body);
    }

    // push remaining bodies as last state
    if (!bodies.empty()) {
        states.emplace_back(current_time, bodies);
    }

    // return the result
    return SimResult{states};
}

void SimResult::saveToMsgPack(const std::string &filename) const {
    // open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // pack data
    msgpack::pack(file, states);

    file.close();
}

auto SimResult::loadFromMsgPack(const std::string &filename) -> SimResult {
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