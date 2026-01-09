#include "viz.hpp"

#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <glviskit/glviskit.hpp>

#include "common/constants.hpp"
#include "common/vec.hpp"

namespace nbodysim {

App::App(const std::vector<SimState> &states)
    : window_(glviskit::CreateWindow("N-Body Simulation", 800, 800)),
      buffer_body_(glviskit::CreateRenderList()),
      buffer_orbit_(glviskit::CreateRenderList()),
      time_start_(glviskit::GetTimeSeconds()) {
    // add render buffers to window
    window_->AddRenderList(buffer_body_);
    window_->AddRenderList(buffer_orbit_);

    // setup camera to a reasonable default position
    auto camera = window_->GetCamera();
    camera->SetPosition({0.0F, 0.0F, 0.0F});
    camera->PerspectiveFov(60.0F, 60.0F, 0.1F, 100.0F);
    camera->SetRotation({-0.6F, 0.0F, 0.0F});
    camera->SetDistance(35.0F);

    // if no states or no bodies, return
    if (states.empty() || states[0].bodies.empty()) {
        return;
    }

    // initialize data from simulation states
    auto t0 = glviskit::GetTimeSeconds();
    body_count_ = states[0].bodies.size();
    state_count_ = states.size();

    // reserve space
    times_.reserve(state_count_);
    positions_.resize(body_count_ * state_count_);
    colors_.reserve(body_count_);
    sizes_.reserve(body_count_);

    // fill times and positions
    for (const auto &state : states) {
        times_.push_back(state.time);
    }

    // we need to store positions in state-major order
    // for better cache locality while rendering
    // this is opposite of how simulation states are stored
    // which is much more natural for simulation
    for (size_t si = 0; si < state_count_; si++) {
        const auto &state = states[si];
        for (size_t bi = 0; bi < body_count_; bi++) {
            const auto &body = state.bodies[bi];
            positionAt(si, bi) = {static_cast<float>(body.pos.x()),
                                  static_cast<float>(body.pos.z()),
                                  static_cast<float>(body.pos.y())};
        }
    }

    // generate colors based on body index
    for (size_t i = 0; i < body_count_; i++) {
        const float hue =
            static_cast<float>(i) / static_cast<float>(body_count_);
        const auto r_angle = hue * 2 * constants::pi;
        const auto g_angle = r_angle + (2 * constants::pi / 3);
        const auto b_angle = r_angle + (4 * constants::pi / 3);

        const float r = std::fabs(std::sin(r_angle));
        const float g = std::fabs(std::sin(g_angle));
        const float b = std::fabs(std::sin(b_angle));
        colors_.emplace_back(r, g, b, 1.0F);
    }

    // generate sizes based on cube root of mass
    // while exact size scaling is arbitrary
    // cube root gives a better visual distinction between masses
    for (const auto &body : states[0].bodies) {
        auto size = static_cast<float>(std::cbrt(body.mass) * size_scale_);
        size = std::clamp(size, size_min_, size_max_);
        sizes_.push_back(size);
    }

    // setup time parameters
    // they are used for interpolation during rendering
    start_time_ = times_.front();
    stop_time_ = times_.back();
    total_time_ = stop_time_ - start_time_;
    dt_ = total_time_ / static_cast<float>(state_count_ - 1);
}

auto App::positionInterpolate(size_t body_idx, float t) -> glm::vec3 {
    // handle edge cases
    if (state_count_ == 0) {
        return {0.0F, 0.0F, 0.0F};
    }
    if (t <= times_.front()) {
        return positionAt(0, body_idx);
    }
    if (t >= times_.back()) {
        return positionAt(state_count_ - 1, body_idx);
    }

    // assume uniform time steps
    const size_t idx = static_cast<size_t>((t - start_time_) / dt_);

    // interpolate linearly between idx and idx + 1
    const float t0 = start_time_ + static_cast<float>(idx) * dt_;
    const float alpha = (t - t0) / dt_;

    const auto &p0 = positionAt(idx, body_idx);
    const auto &p1 = positionAt(idx + 1, body_idx);

    return {p0.x * (1.0F - alpha) + p1.x * alpha,
            p0.y * (1.0F - alpha) + p1.y * alpha,
            p0.z * (1.0F - alpha) + p1.z * alpha};
}

void App::run() {
    // main loop
    while (glviskit::Loop()) {
        draw();
    }
}

void App::draw() {
    // current sim acquire time
    auto t = glviskit::GetTimeSeconds() - time_start_;
    auto t_sim = t * speed_factor_;

    // if simulation time exceeds total time, wrap around
    if (t_sim > total_time_) {
        t_sim = std::fmodf(t_sim, total_time_);
        time_start_ = glviskit::GetTimeSeconds();
        state_idx_prev_ = 0;
        state_idx_curr_ = 0;
        // since we are not clearing orbit buffer but append continually
        // we need to clear it when restarting simulation visualization
        buffer_orbit_->Clear();
    }

    // draw bodies with trails
    // since using prev frame info not really feasible
    // we are removing old body and trail
    buffer_body_->Clear();

    // loop over bodies and states
    for (size_t i = 0; i < body_count_; i++) {
        auto &color = colors_[i];
        auto &size = sizes_[i];
        buffer_body_->Size(size);

        // draw trail of each body by interpolating positions
        // backwards in time
        for (unsigned j = 0; j <= trail_segments_; j++) {
            const float alpha =
                (static_cast<float>(j) / static_cast<float>(trail_segments_));
            const float t_trail = t_sim - (trail_length_ * alpha);
            const auto p = positionInterpolate(i, t_trail);

            buffer_body_->Color(
                {color.r, color.g, color.b, std::expf(-3.0F * alpha)});
            buffer_body_->LineTo(p);
        }
        buffer_body_->LineEnd();

        // draw current body position
        // again by interpolation for smooth animation
        auto p = positionInterpolate(i, t_sim);
        buffer_body_->Color({color.r, color.g, color.b, 1.0F});
        buffer_body_->Circle(p);
    }

    // update orbit trails
    // determine current state index
    state_idx_curr_ = static_cast<size_t>((t_sim - start_time_) / dt_);
    if (state_idx_curr_ >= state_count_) {
        state_idx_curr_ = state_count_ - 1;
    }

    // append new line segments to orbit buffer
    buffer_orbit_->Size(orbit_width_);
    for (size_t i = 0; i < body_count_; i++) {
        auto &color = colors_[i];
        auto &size = sizes_[i];

        for (int j = state_idx_prev_ + 1; j <= state_idx_curr_; j++) {
            const auto p1 = positionAt(j - 1, i);
            const auto p2 = positionAt(j, i);

            buffer_orbit_->Color({color.r, color.g, color.b, 0.4F});
            buffer_orbit_->Line(p1, p2);
        }
    }
    state_idx_prev_ = state_idx_curr_;
}
}  // namespace nbodysim