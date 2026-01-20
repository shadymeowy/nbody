#include "viz.hpp"

#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <glviskit/glviskit.hpp>
#include <memory>

#include "common/constants.hpp"
#include "common/vec.hpp"

namespace viz = glviskit;

namespace nbodysim {

VizApp::VizApp(const SimResult &result)
    : window_(viz::CreateWindow("N-Body Simulation", 800, 800)),
      list_body_(viz::CreateRenderList()),
      list_orbit_(viz::CreateRenderList()),
      list_octree_(viz::CreateRenderList()),
      time_start_(viz::GetTimeSeconds()) {
    // get states reference for convenience
    const auto &states = result.states;

    // add render buffers to window
    window_->AddRenderList(list_body_);
    window_->AddRenderList(list_orbit_);
    window_->AddRenderList(list_octree_);

    // setup camera to a reasonable default position
    auto camera = window_->GetCamera();
    camera->SetPosition({0.0F, 0.0F, 0.0F});
    camera->PerspectiveFov(60.0F, 60.0F, 0.1F, 1000.0F);
    camera->SetRotation({0.0F, 0.0F, 0.0F});
    camera->SetDistance(0.0F);

    auto controller = std::make_shared<viz::SphericalController>();
    window_->SetController(controller);

    // if no states or no bodies, return
    if (states.empty() || states[0].bodies.empty()) {
        return;
    }

    // setup visualization parameters
    setupSimResult(result);
    // setup other viz parameters
    setupVizParameters();
    // setup orbit paths
    setupOrbitPaths();
}

void VizApp::setupSimResult(const SimResult &result) {
    const auto &states = result.states;

    // initialize data from simulation states
    auto t0 = viz::GetTimeSeconds();
    body_count_ = states[0].bodies.size();
    state_count_ = states.size();

    // reserve space
    times_.clear();
    times_.reserve(state_count_);
    positions_.clear();
    positions_.resize(body_count_ * state_count_);

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

    // save masses for size calculation
    masses_.clear();
    masses_.reserve(body_count_);
    for (const auto &body : states[0].bodies) {
        masses_.push_back(static_cast<float>(body.mass));
    }

    // load octree cubes if available
    for (const auto &state : states) {
        std::vector<Cube> cubes;
        for (const auto &cube : state.octree_cubes) {
            for (const auto &bcube : cube) {
                Cube c;
                c.center = glm::vec3{static_cast<float>(bcube.center.v[0]),
                                     static_cast<float>(bcube.center.v[2]),
                                     static_cast<float>(bcube.center.v[1])};
                c.half_size = static_cast<float>(bcube.half_size);
                cubes.push_back(c);
            }
        }
        octree_cubes_.push_back(cubes);
    }
}

void VizApp::setupVizParameters() {
    // generate sizes and colors
    colors_.clear();
    colors_.reserve(body_count_);
    sizes_.clear();
    sizes_.reserve(body_count_);

    // generate sizes based on cube root of mass
    // while exact size scaling is arbitrary
    // cube root gives a better visual distinction between masses
    for (const auto &mass : masses_) {
        auto size = static_cast<float>(std::cbrt(mass) * size_scale_);
        size = std::clamp(size, size_min_, size_max_);
        sizes_.push_back(size);
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
}

void VizApp::setupOrbitPaths() {
    // clear existing orbit paths
    paths_orbit_.clear();

    // clear orbit render list
    list_orbit_->Clear();

    // rewind state index trackers
    state_idx_prev_ = 0;
    state_idx_curr_ = 0;

    // insert paths into orbit buffer
    for (size_t i = 0; i < body_count_; i++) {
        auto &color = colors_[i];
        auto path = list_orbit_->PathBegin();

        path->Size(orbit_width_);
        path->Color({color.r, color.g, color.b, 0.4F});
        paths_orbit_.push_back(path);
    }

    // save orbit path state so we can restore if
    // simulation visualization restarts
    list_orbit_->Save();
    // set initial orbit visibility
    list_orbit_->SetEnabled(show_orbits_);

    // setup time parameters
    // they are used for interpolation during rendering
    start_time_ = times_.front();
    stop_time_ = times_.back();
    total_time_ = stop_time_ - start_time_;
    dt_ = total_time_ / static_cast<float>(state_count_ - 1);
}

auto VizApp::positionInterpolate(size_t body_idx, float t) -> glm::vec3 {
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

void VizApp::run() {
    // main loop
    while (viz::Loop()) {
        draw();
    }
}

void VizApp::draw() {
    // current sim acquire time
    auto t = viz::GetTimeSeconds() - time_start_;
    auto t_sim = t * speed_factor_;

    // if simulation time exceeds total time, wrap around
    if (t_sim > total_time_) {
        t_sim = std::fmod(t_sim, total_time_);
        time_start_ = viz::GetTimeSeconds();
        state_idx_prev_ = 0;
        state_idx_curr_ = 0;
        // since we are not clearing orbit buffer but append continually
        // we need to clear it when restarting simulation visualization
        list_orbit_->Restore();
    }

    // draw bodies with trails
    // since using prev frame info not really feasible
    // we are removing old body and trail
    list_body_->Clear();

    // loop over bodies and states
    auto path = list_body_->PathBegin();
    for (size_t i = 0; i < body_count_; i++) {
        auto &color = colors_[i];
        auto &size = sizes_[i];

        // draw trail of each body by interpolating positions
        // backwards in time
        path->Size(size);
        for (unsigned j = 0; j <= trail_segments_; j++) {
            const float alpha =
                (static_cast<float>(j) / static_cast<float>(trail_segments_));
            const float t_trail = t_sim - (trail_length_ * alpha);
            const auto p = positionInterpolate(i, t_trail);

            path->Color({color.r, color.g, color.b, std::exp(-3.0F * alpha)});
            path->LineTo(p);
        }
        path->LineEnd();

        // draw current body position
        // again by interpolation for smooth animation
        auto p = positionInterpolate(i, t_sim);
        list_body_->Size(size);
        list_body_->Color({color.r, color.g, color.b, 1.0F});
        list_body_->Circle(p);
    }

    // update orbit trails
    // determine current state index
    state_idx_curr_ = static_cast<size_t>((t_sim - start_time_) / dt_);
    if (state_idx_curr_ >= state_count_) {
        state_idx_curr_ = state_count_ - 1;
    }

    // append new orbit segments
    if (show_orbits_) {
        for (size_t i = 0; i < body_count_; i++) {
            auto &path = paths_orbit_[i];

            for (int j = state_idx_prev_; j <= state_idx_curr_; j++) {
                const auto p = positionAt(j, i);
                path->LineTo(p);
            }
        }
        state_idx_prev_ = state_idx_curr_;
    }

    // draw octree if enabled
    if (show_octree_) {
        drawOctree(t_sim);
    }
}

void VizApp::drawOctree(float t_sim) {
    // draw octree cubes
    list_octree_->Clear();

    // find closest recorded state index
    size_t state_idx = static_cast<size_t>((t_sim - start_time_) / dt_);
    if (state_idx >= state_count_) {
        state_idx = state_count_ - 1;
    }

    for (const auto &cube : octree_cubes_[state_idx]) {
        list_octree_->Color({1.0F, 0.2F, 0.2F, 0.4F});
        list_octree_->Size(2.0F);
        std::array<glm::vec3, 8> corners;
        const auto &c = cube.center;
        const float hs = cube.half_size;

        corners[0] = {c.x - hs, c.y - hs, c.z - hs};
        corners[1] = {c.x + hs, c.y - hs, c.z - hs};
        corners[2] = {c.x + hs, c.y + hs, c.z - hs};
        corners[3] = {c.x - hs, c.y + hs, c.z - hs};
        corners[4] = {c.x - hs, c.y - hs, c.z + hs};
        corners[5] = {c.x + hs, c.y - hs, c.z + hs};
        corners[6] = {c.x + hs, c.y + hs, c.z + hs};
        corners[7] = {c.x - hs, c.y + hs, c.z + hs};

        // draw edges
        list_octree_->Line(corners[0], corners[1]);
        list_octree_->Line(corners[1], corners[2]);
        list_octree_->Line(corners[2], corners[3]);
        list_octree_->Line(corners[3], corners[0]);
        list_octree_->Line(corners[4], corners[5]);
        list_octree_->Line(corners[5], corners[6]);
        list_octree_->Line(corners[6], corners[7]);
        list_octree_->Line(corners[7], corners[4]);
        list_octree_->Line(corners[0], corners[4]);
        list_octree_->Line(corners[1], corners[5]);
        list_octree_->Line(corners[2], corners[6]);
        list_octree_->Line(corners[3], corners[7]);
    }
}
}  // namespace nbodysim