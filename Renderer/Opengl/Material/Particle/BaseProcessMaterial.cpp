#include "BaseProcessMaterial.h"

namespace Material {
    BaseProcessMaterial::BaseProcessMaterial(const shared_ptr<ResourceManager> &resource_manager)
        : resourceManager(resource_manager) {
    }

    float BaseProcessMaterial::get_life_min() const {
        return lifeMin;
    }

    void BaseProcessMaterial::set_life_min(const float life_min) {
        lifeMin = life_min;
    }

    float BaseProcessMaterial::get_life_max() const {
        return lifeMax;
    }

    void BaseProcessMaterial::set_life_max(const float life_max) {
        lifeMax = life_max;
    }

    float BaseProcessMaterial::get_size_min() const {
        return sizeMin;
    }

    void BaseProcessMaterial::set_size_min(const float size_min) {
        sizeMin = size_min;
    }

    float BaseProcessMaterial::get_size_max() const {
        return sizeMax;
    }

    void BaseProcessMaterial::set_size_max(const float size_max) {
        sizeMax = size_max;
    }

    float BaseProcessMaterial::get_stretch() const {
        return stretch;
    }

    void BaseProcessMaterial::set_stretch(const float stretch) {
        this->stretch = stretch;
    }

    glm::vec3 BaseProcessMaterial::get_vel_min() const {
        return velMin;
    }

    void BaseProcessMaterial::set_vel_min(const glm::vec3 &vel_min) {
        velMin = vel_min;
    }

    glm::vec3 BaseProcessMaterial::get_vel_max() const {
        return velMax;
    }

    void BaseProcessMaterial::set_vel_max(const glm::vec3 &vel_max) {
        velMax = vel_max;
    }

    glm::vec3 BaseProcessMaterial::get_gravity() const {
        return gravity;
    }

    void BaseProcessMaterial::set_gravity(const glm::vec3 &gravity) {
        this->gravity = gravity;
    }

    float BaseProcessMaterial::get_emitter_radius() const {
        return emitterRadius;
    }

    void BaseProcessMaterial::set_emitter_radius(const float emitter_radius) {
        emitterRadius = emitter_radius;
    }

    float BaseProcessMaterial::get_emitter_y_offset() const {
        return emitterYOffset;
    }

    void BaseProcessMaterial::set_emitter_y_offset(const float emitter_y_offset) {
        emitterYOffset = emitter_y_offset;
    }

    glm::vec4 BaseProcessMaterial::get_color_start() const {
        return colorStart;
    }

    void BaseProcessMaterial::set_color_start(const glm::vec4 &color_start) {
        colorStart = color_start;
    }

    glm::vec4 BaseProcessMaterial::get_color_end() const {
        return colorEnd;
    }

    void BaseProcessMaterial::set_color_end(const glm::vec4 &color_end) {
        colorEnd = color_end;
    }

    float BaseProcessMaterial::get_spawn_per_frame() const {
        return spawnPerFrame;
    }

    void BaseProcessMaterial::set_spawn_per_frame(const float spawn_per_frame) {
        spawnPerFrame = spawn_per_frame;
    }

    bool BaseProcessMaterial::is_smooth_start() const {
        return smoothStart;
    }

    void BaseProcessMaterial::set_smooth_start(const bool smooth_start) {
        smoothStart = smooth_start;
    }

    float BaseProcessMaterial::get_warmup_time() const {
        return warmupTime;
    }

    void BaseProcessMaterial::set_warmup_time(const float warmup_time) {
        warmupTime = warmup_time;
    }

    int BaseProcessMaterial::get_warmup_substeps() const {
        return warmupSubsteps;
    }

    void BaseProcessMaterial::set_warmup_substeps(const int warmup_substeps) {
        warmupSubsteps = warmup_substeps;
    }

    float BaseProcessMaterial::get_first_frame_clamp() const {
        return firstFrameClamp;
    }

    void BaseProcessMaterial::set_first_frame_clamp(const float first_frame_clamp) {
        firstFrameClamp = first_frame_clamp;
    }

    float BaseProcessMaterial::get_color_sensitivity() const {
        return colorSensitivity;
    }

    void BaseProcessMaterial::set_color_sensitivity(const float color_sensitivity) {
        colorSensitivity = color_sensitivity;
    }

    glm::vec2 BaseProcessMaterial::get_turbulence() const {
        return turbulence;
    }

    void BaseProcessMaterial::set_turbulence(const glm::vec2 &turbulence) {
        this->turbulence = turbulence;
    }

    int BaseProcessMaterial::get_spawn_shape() const {
        return spawnShape;
    }

    void BaseProcessMaterial::set_spawn_shape(const int spawn_shape) {
        spawnShape = spawn_shape;
    }

    int BaseProcessMaterial::get_respawn_mode() const {
        return respawnMode;
    }

    void BaseProcessMaterial::set_respawn_mode(const int respawn_mode) {
        respawnMode = respawn_mode;
    }

    float BaseProcessMaterial::get_min_radius() const {
        return minRadius;
    }

    void BaseProcessMaterial::set_min_radius(const float min_radius) {
        minRadius = min_radius;
    }

    float BaseProcessMaterial::get_max_radius() const {
        return maxRadius;
    }

    void BaseProcessMaterial::set_max_radius(const float max_radius) {
        maxRadius = max_radius;
    }

    float BaseProcessMaterial::get_spawn_height() const {
        return spawnHeight;
    }

    void BaseProcessMaterial::set_spawn_height(const float spawn_height) {
        spawnHeight = spawn_height;
    }

    float BaseProcessMaterial::get_time_offset() const {
        return timeOffset;
    }

    void BaseProcessMaterial::set_time_offset(const float time_offset) {
        timeOffset = time_offset;
    }

    std::string BaseProcessMaterial::get_texture() const {
        return texture;
    }

    void BaseProcessMaterial::set_texture(const std::string &texture) {
        this->texture = texture;
    }

    ParticleMode BaseProcessMaterial::get_mode() const {
        return mode;
    }

    void BaseProcessMaterial::set_mode(const ParticleMode mode) {
        this->mode = mode;
    }

    glm::vec3 BaseProcessMaterial::get_emitter_pos() const {
        return emitterPos;
    }

    void BaseProcessMaterial::set_emitter_pos(const glm::vec3 &emitter_pos) {
        emitterPos = emitter_pos;
    }

    glm::vec2 BaseProcessMaterial::get_emitter_size() const {
        return emitterSize;
    }

    void BaseProcessMaterial::set_emitter_size(const glm::vec2 &emitter_size) {
        emitterSize = emitter_size;
    }

    float BaseProcessMaterial::get_drag() const {
        return drag;
    }

    void BaseProcessMaterial::set_drag(const float drag) {
        this->drag = drag;
    }
} // Material