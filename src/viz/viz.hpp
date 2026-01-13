#pragma once

#include <glm/glm.hpp>
#include <glviskit/glviskit.hpp>
#include <vector>

#include "glviskit/render_list.hpp"
#include "glviskit/sdl/window.hpp"
#include "integrator/sim_state.hpp"

namespace nbodysim {

class App {
   public:
    explicit App(const std::vector<SimState> &states);

    void run();
    void draw();

    void setShowOrbits(bool show_orbits) {
        show_orbits_ = show_orbits;
        list_orbit_->SetEnabled(show_orbits_);
    }
    auto getShowOrbits() const -> bool {
        return show_orbits_;
    }

    void setSpeedFactor(float speed_factor) {
        speed_factor_ = speed_factor;
    }
    auto setSpeedFactor() const -> float {
        return speed_factor_;
    }

    void setTrailLength(float trail_length) {
        trail_length_ = trail_length;
    }
    auto getTrailLength() const -> float {
        return trail_length_;
    }

    void setTrailSegments(unsigned trail_segments) {
        trail_segments_ = trail_segments;
    }
    auto getTrailSegments() const -> unsigned {
        return trail_segments_;
    }

    void setSizeMin(float size_min) {
        size_min_ = size_min;
    }
    auto getSizeMin() const -> float {
        return size_min_;
    }

    void setSizeMax(float size_max) {
        size_max_ = size_max;
    }
    auto getSizeMax() const -> float {
        return size_max_;
    }

    void setSizeScale(float size_scale) {
        size_scale_ = size_scale;
    }
    auto getSizeScale() const -> float {
        return size_scale_;
    }

    void setOrbitWidth(float orbit_width) {
        orbit_width_ = orbit_width;
    }
    auto getOrbitWidth() const -> float {
        return orbit_width_;
    }

    auto &positionAt(size_t state_idx, size_t body_idx) {
        return positions_[body_idx * state_count_ + state_idx];
    }

    auto positionInterpolate(size_t body_idx, float t) -> glm::vec3;

   private:
    size_t body_count_ = 0;
    size_t state_count_ = 0;
    std::vector<float> times_;
    std::vector<glm::vec3> positions_;
    std::vector<glm::vec4> colors_;
    std::vector<float> sizes_;

    float start_time_ = 0.0F;
    float stop_time_ = 0.0F;
    float total_time_ = 0.0F;
    float dt_ = 0.0F;
    size_t state_idx_prev_ = 0;
    size_t state_idx_curr_ = 0;

    bool show_orbits_ = false;
    float speed_factor_ = 1.0F;
    float trail_length_ = 1.0F;
    unsigned trail_segments_ = 10;
    float size_min_ = 2.0F;
    float size_max_ = 32.0F;
    float size_scale_ = 300.0F;
    float orbit_width_ = 3.0F;

    std::shared_ptr<glviskit::sdl::Window> window_;
    std::shared_ptr<glviskit::RenderList> list_body_;
    std::shared_ptr<glviskit::RenderList> list_orbit_;
    std::vector<std::shared_ptr<glviskit::Path>> paths_orbit_;
    float time_start_ = 0.0F;
};

}  // namespace nbodysim