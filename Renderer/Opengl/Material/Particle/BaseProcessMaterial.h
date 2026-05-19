#ifndef SNAKE3_BASEPROCESSMATERIAL_H
#define SNAKE3_BASEPROCESSMATERIAL_H

#include <string>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../../../../Manager/ResourceManager.h"

using namespace std;

namespace Material {
    class BaseProcessMaterial;

    enum ParticleMode {
        Mesh3D = 2,
        Billboard = 1,
        Stretched = 0
    };

    class BaseProcessMaterial {
    public:
        virtual ~BaseProcessMaterial() = default;

        explicit BaseProcessMaterial(const shared_ptr<ResourceManager> &resource_manager);

        [[nodiscard]] float get_life_min() const;

        void set_life_min(float life_min);

        [[nodiscard]] float get_life_max() const;

        void set_life_max(float life_max);

        [[nodiscard]] float get_size_min() const;

        void set_size_min(float size_min);

        [[nodiscard]] float get_size_max() const;

        void set_size_max(float size_max);

        [[nodiscard]] float get_stretch() const;

        void set_stretch(float stretch);

        [[nodiscard]] glm::vec3 get_vel_min() const;

        void set_vel_min(const glm::vec3 &vel_min);

        [[nodiscard]] glm::vec3 get_vel_max() const;

        void set_vel_max(const glm::vec3 &vel_max);

        [[nodiscard]] glm::vec3 get_gravity() const;

        void set_gravity(const glm::vec3 &gravity);

        [[nodiscard]] float get_emitter_radius() const;

        void set_emitter_radius(float emitter_radius);

        [[nodiscard]] float get_emitter_y_offset() const;

        void set_emitter_y_offset(float emitter_y_offset);

        [[nodiscard]] glm::vec4 get_color_start() const;

        void set_color_start(const glm::vec4 &color_start);

        [[nodiscard]] glm::vec4 get_color_end() const;

        void set_color_end(const glm::vec4 &color_end);

        [[nodiscard]] float get_spawn_per_frame() const;

        void set_spawn_per_frame(float spawn_per_frame);

        [[nodiscard]] bool is_smooth_start() const;

        void set_smooth_start(bool smooth_start);

        [[nodiscard]] float get_warmup_time() const;

        void set_warmup_time(float warmup_time);

        [[nodiscard]] int get_warmup_substeps() const;

        void set_warmup_substeps(int warmup_substeps);

        [[nodiscard]] float get_first_frame_clamp() const;

        void set_first_frame_clamp(float first_frame_clamp);

        [[nodiscard]] float get_color_sensitivity() const;

        void set_color_sensitivity(float color_sensitivity);

        [[nodiscard]] glm::vec2 get_turbulence() const;

        void set_turbulence(const glm::vec2 &turbulence);

        [[nodiscard]] int get_spawn_shape() const;

        void set_spawn_shape(int spawn_shape);

        [[nodiscard]] int get_respawn_mode() const;

        void set_respawn_mode(int respawn_mode);

        [[nodiscard]] float get_min_radius() const;

        void set_min_radius(float min_radius);

        [[nodiscard]] float get_max_radius() const;

        void set_max_radius(float max_radius);

        [[nodiscard]] float get_spawn_height() const;

        void set_spawn_height(float spawn_height);

        [[nodiscard]] float get_time_offset() const;

        void set_time_offset(float time_offset);

        [[nodiscard]] std::string get_texture() const;

        void set_texture(const std::string &texture);

        [[nodiscard]] ParticleMode get_mode() const;

        void set_mode(ParticleMode mode);

        [[nodiscard]] glm::vec3 get_emitter_pos() const;

        void set_emitter_pos(const glm::vec3 &emitter_pos);

        [[nodiscard]] glm::vec2 get_emitter_size() const;

        void set_emitter_size(const glm::vec2 &emitter_size);

        [[nodiscard]] float get_drag() const;

        void set_drag(float drag);

        [[nodiscard]] float get_burst_interval() const;

        void set_burst_interval(float burst_interval);

        [[nodiscard]] float get_spawn_window() const;

        void set_spawn_window(float spawn_window);

        virtual void bind(shared_ptr<ShaderProgram> shader) const = 0;

        virtual void update(shared_ptr<ShaderProgram> shader, int maxParticles, float timeAccum, float timeOffset, float stepDt) = 0;

    protected:
        float lifeMin = 1.0f;
        float lifeMax = 2.0f;
        float sizeMin = 0.02f;
        float sizeMax = 0.08f;
        float stretch = 0.15f;

        // rychlost a gravitace
        glm::vec3 velMin = {-0.1f, 0.6f, -0.1f};
        glm::vec3 velMax = {0.1f, 1.6f, 0.1f};
        glm::vec3 gravity = {0.0f, -0.4f, 0.0f};

        // kruhovy emitor
        glm::vec3 emitterPos = {0.0f, 0.0f, 0.0f};
        glm::vec2 emitterSize = {0.0f, 0.0f};
        float emitterRadius = 0.05f;
        float emitterYOffset = 0.0f;

        // barvy
        glm::vec4 colorStart = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 colorEnd = {1.0f, 1.0f, 1.0f, 0.0f};

        float spawnPerFrame = 1.0f;
        bool smoothStart = true;
        float warmupTime = 2.5f; // doba náběhu (s)
        int warmupSubsteps = 4; // počet dílčích kroků v prvním snímku
        float firstFrameClamp = 1.0f / 30.0f; // maximální dt na prvním snímku
        float colorSensitivity = 1.0f;

        glm::vec2 turbulence = {0.0f, 0.0f}; // x=sila, y=frekvence
        int spawnShape = 0; // 0 = Local, 1 = Environment Ring, 2 = Sphere
        int respawnMode = 0; // 0 = Die, 1 = Wrap infinite
        float minRadius = 0.0f; // vnitrni polomer (safe zone)
        float maxRadius = 1.0f; // vnejsi polomer (area)
        float spawnHeight = 10.0f; // vyska sloupce
        float timeOffset = 0.0f;
        float drag = 0.0f;
        float burstInterval = 3.0f;
        float spawnWindow = 0.1f;

        std::string texture;
        ParticleMode mode = Billboard;

        shared_ptr<ResourceManager> resourceManager;
    };
} // Material

#endif //SNAKE3_BASEPROCESSMATERIAL_H
