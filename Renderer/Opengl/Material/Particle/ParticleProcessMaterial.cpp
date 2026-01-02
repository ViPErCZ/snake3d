#include "ParticleProcessMaterial.h"

namespace Material {
    ParticleProcessMaterial::ParticleProcessMaterial(const shared_ptr<ResourceManager> &resource_manager)
        : BaseProcessMaterial(resource_manager) {
        update_shader = resourceManager->getShader("particle_update");
        render_shader = resourceManager->getShader("particle_3d_render");
        render_texture_shader = resourceManager->getShader("particle_3d_render_tex");
    }

    void ParticleProcessMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                                       const glm::mat4 &model, bool shadows) const {

        const auto shader = !texture.empty() ? render_texture_shader : render_shader;

        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("model", mode == Billboard ? glm::mat4(1.0f) : model);
        shader->setInt("u_mode", mode);
        shader->setFloat("u_lifeMin", lifeMin);
        shader->setFloat("u_lifeMax", lifeMax);
        shader->setFloat("u_sizeMin", sizeMin);
        shader->setFloat("u_sizeMax", sizeMax);
        shader->setFloat("u_stretch", stretch);
        shader->setVec4("u_colorStart", colorStart);
        shader->setVec4("u_colorEnd", colorEnd);
        shader->setFloat("u_colorSensitivity", colorSensitivity);
        if (!texture.empty()) {
            shader->setInt("uTexture0", 0);
            resourceManager->getTexture(texture)->bind();
        }
    }

    void ParticleProcessMaterial::update(const int maxParticles, const float timeAccum,
                                         const float timeOffset, const float stepDt) {
        update_shader->use();
        update_shader->setFloat("u_dt", stepDt);
        update_shader->setBool("u_is2D", false);
        update_shader->setFloat("u_timeAccum", timeAccum + timeOffset);
        update_shader->setVec3("u_emitterPos", emitterPos);

        update_shader->setInt("u_spawnShape", spawnShape);
        update_shader->setInt("u_respawnMode", respawnMode);
        update_shader->setVec2("u_turbulence", turbulence);
        update_shader->setFloat("u_minRadius", minRadius);
        update_shader->setFloat("u_maxRadius", maxRadius);
        update_shader->setFloat("u_spawnHeight", spawnHeight);

        // update_shader->setInt("u_mode", mode);
        update_shader->setFloat("u_lifeMin", lifeMin);
        update_shader->setFloat("u_lifeMax", lifeMax);
        update_shader->setFloat("u_sizeMin", sizeMin);
        update_shader->setFloat("u_sizeMax", sizeMax);
        update_shader->setVec3("u_velMin", velMin);
        update_shader->setVec3("u_velMax", velMax);
        update_shader->setVec3("u_gravity", gravity);
        update_shader->setFloat("u_emitterRadius", emitterRadius);
        update_shader->setFloat("u_emitterYOffset", emitterYOffset);
        update_shader->setFloat("u_spawnPerFrame", spawnPerFrame);
    }
} // Material
