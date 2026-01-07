#include "viz.hpp"

#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <glviskit/glviskit.hpp>

#include "common/vec.hpp"
#include "glm/fwd.hpp"

namespace nbodysim::viz {
void visualizeSimulation(const std::vector<SimState> &states,
                         float speed_factor, float trail_length,
                         unsigned trail_segments, float size_min,
                         float size_max, float size_scale) {
    if (states.empty()) {
        return;
    }

    auto window = glviskit::CreateWindow("N-Body Simulation", 800, 800);
    auto buffer = glviskit::CreateRenderBuffer();
    window->AddRenderBuffer(buffer);

    auto t0 = glviskit::GetTimeSeconds();

    auto camera = window->GetCamera();
    camera->SetPosition({0.0F, 0.0F, 0.0F});
    camera->PerspectiveFov(60.0F, 60.0F, 0.1F, 1000.0F);
    camera->SetRotation({-0.6F, 0.0F, 0.0F});
    camera->SetDistance(35.0F);

    std::vector<glm::vec4> colors;
    colors.reserve(states[0].bodies.size());
    for (size_t i = 0; i < states[0].bodies.size(); i++) {
        const float hue =
            static_cast<float>(i) / static_cast<float>(states[0].bodies.size());
        const float r = std::fabs(std::sin(hue * 6.2831853F + 0.0F));
        const float g = std::fabs(std::sin(hue * 6.2831853F + 2.0943951F));
        const float b = std::fabs(std::sin(hue * 6.2831853F + 4.1887902F));
        colors.emplace_back(r, g, b, 1.0F);
    }

    std::vector<float> sizes;
    sizes.reserve(states[0].bodies.size());
    for (const auto &body : states[0].bodies) {
        auto size = static_cast<float>(std::cbrt(body.mass) * size_scale);
        size = std::clamp(size, size_min, size_max);
        sizes.push_back(size);
    }

    const float start_time = states.front().time;
    const float stop_time = states.back().time;
    const float total_time = stop_time - start_time;
    const float dt = total_time / static_cast<float>(states.size() - 1);

    auto getBody = [&](const std::vector<SimState> &states, size_t body_idx,
                       float t) -> Vec3 {
        assert(body_idx < states[0].bodies.size());

        if (t <= start_time) {
            return states.front().bodies[body_idx].pos;
        }
        if (t >= stop_time) {
            return states.back().bodies[body_idx].pos;
        }

        // assume uniform time steps
        const size_t idx = static_cast<size_t>((t - start_time) / dt);

        // interpolate linearly between idx and idx + 1
        const float t0 = start_time + static_cast<float>(idx) * dt;
        const float alpha = (t - t0) / dt;

        const Body &b0 = states[idx].bodies[body_idx];
        const Body &b1 = states[idx + 1].bodies[body_idx];

        return {b0.pos.x() * (1.0F - alpha) + b1.pos.x() * alpha,
                b0.pos.y() * (1.0F - alpha) + b1.pos.y() * alpha,
                b0.pos.z() * (1.0F - alpha) + b1.pos.z() * alpha};
    };

    while (glviskit::Loop()) {
        auto t = glviskit::GetTimeSeconds() - t0;
        auto t_sim = t * speed_factor;

        buffer->Clear();
        for (size_t i = 0; i < states[0].bodies.size(); i++) {
            auto &color = colors[i];
            auto &size = sizes[i];

            buffer->Size(size);

            for (unsigned j = 0; j <= trail_segments; j++) {
                const float alpha = (static_cast<float>(j) /
                                     static_cast<float>(trail_segments));
                const float t_trail = t_sim - (trail_length * alpha);
                const auto p = getBody(states, i, t_trail);

                buffer->Color(
                    {color.r, color.g, color.b, std::expf(-3.0F * alpha)});
                buffer->LineTo({static_cast<float>(p.x()),
                                static_cast<float>(p.z()),
                                static_cast<float>(p.y())});
            }
            buffer->LineEnd();

            auto p = getBody(states, i, t_sim);
            buffer->Color({color.r, color.g, color.b, 1.0F});
            buffer->Circle({static_cast<float>(p.x()),
                            static_cast<float>(p.z()),
                            static_cast<float>(p.y())});
        }
    }
}

}  // namespace nbodysim::viz