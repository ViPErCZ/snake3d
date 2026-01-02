#include "ParticleProcessMaterial.h"

namespace Material {
    ParticleProcessMaterial::ParticleProcessMaterial(const shared_ptr<ResourceManager> &resource_manager)
        : BaseProcessMaterial(resource_manager) {
    }

    void ParticleProcessMaterial::bind(const shared_ptr<ShaderManager> shader, const glm::vec3 &posView,
                                       const glm::mat4 &view, const glm::mat4 &projection,
                                       const glm::mat4 &model, bool shadows) const {
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

    void ParticleProcessMaterial::update(const shared_ptr<ShaderManager> shader, const int maxParticles,
                                         const float timeAccum,
                                         const float timeOffset, const float stepDt) {
        shader->use();
        shader->setFloat("u_dt", stepDt);
        shader->setBool("u_is2D", false);
        shader->setFloat("u_timeAccum", timeAccum + timeOffset);
        shader->setVec3("u_emitterPos", emitterPos);

        shader->setInt("u_spawnShape", spawnShape);
        shader->setInt("u_respawnMode", respawnMode);
        shader->setVec2("u_turbulence", turbulence);
        shader->setFloat("u_minRadius", minRadius);
        shader->setFloat("u_maxRadius", maxRadius);
        shader->setFloat("u_spawnHeight", spawnHeight);

        // shader->setInt("u_mode", mode);
        shader->setFloat("u_lifeMin", lifeMin);
        shader->setFloat("u_lifeMax", lifeMax);
        shader->setFloat("u_sizeMin", sizeMin);
        shader->setFloat("u_sizeMax", sizeMax);
        shader->setVec3("u_velMin", velMin);
        shader->setVec3("u_velMax", velMax);
        shader->setVec3("u_gravity", gravity);
        shader->setFloat("u_emitterRadius", emitterRadius);
        shader->setFloat("u_emitterYOffset", emitterYOffset);
        shader->setFloat("u_spawnPerFrame", spawnPerFrame);
    }
} // Material
