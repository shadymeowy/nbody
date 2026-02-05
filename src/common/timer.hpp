#pragma once

#include <spdlog/spdlog.h>

#include <chrono>

// this file contains timing utilities for measuring execution time of code
// blocks

namespace nbodysim {

class Timer {
   public:
    Timer(const std::string &name)
        : name_(name), start_time_(std::chrono::high_resolution_clock::now()) {}

    // unmovable and uncopyable
    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;
    Timer(Timer &&) = delete;
    Timer &operator=(Timer &&) = delete;
    // print elapsed time on destruction
    ~Timer() {
        elapsed();
    }

    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    void elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time_;
        spdlog::debug("[{}] Elapsed time: {} seconds", name_, diff.count());
    }

   private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::string name_;
};

}  // namespace nbodysim