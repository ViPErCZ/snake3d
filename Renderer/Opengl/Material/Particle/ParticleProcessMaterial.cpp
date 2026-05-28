#include "ParticleProcessMaterial.h"

#include "../../../../Manager/UboBindings.h"

using namespace std;
using namespace Manager;

namespace Material {
    ParticleProcessMaterial::~ParticleProcessMaterial() {
        glDeleteBuffers(1, &uboID);
    }

    ParticleProcessMaterial::ParticleProcessMaterial(const shared_ptr<ResourceManager> &resource_manager)
        : BaseProcessMaterial(resource_manager) {
        glGenBuffers(1, &uboID);
        glBindBuffer(GL_UNIFORM_BUFFER, uboID);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ParticleDataGPU), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, Manager::UBO_BINDING_PARTICLES, uboID);
    }

    void ParticleProcessMaterial::bind(const shared_ptr<ShaderProgram> shader) const {
        shader->use();
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

    void ParticleProcessMaterial::update(const shared_ptr<ShaderProgram> shader, const int maxParticles,
                                         const float timeAccum, const float timeOffset, const float stepDt) {
        const ParticleDataGPU gpuData = prepareUniformData();

        glBindBuffer(GL_UNIFORM_BUFFER, uboID);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ParticleDataGPU), &gpuData);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, Manager::UBO_BINDING_PARTICLES, uboID);

        shader->use();
        shader->setUniformBlock("ParticleParams", Manager::UBO_BINDING_PARTICLES);
        shader->setFloat("u_dt", stepDt);
        shader->setFloat("u_timeAccum", timeAccum + timeOffset);
        shader->setFloat("u_spawnPerFrame", spawnPerFrame);
        shader->setFloat("u_burstInterval", burstInterval);
        shader->setFloat("u_spawnWindow", spawnWindow);
    }

    ParticleProcessMaterial::ParticleDataGPU ParticleProcessMaterial::prepareUniformData() const {
        ParticleDataGPU data{};

        // 1. Life & Size (bez stretch)
        data.u_lifeSizeStretch = glm::vec4(lifeMin, lifeMax, sizeMin, sizeMax);

        // 2. Velocity Min + Stretch (packed)
        data.u_velMinStretch = glm::vec4(velMin, stretch);

        // 3. Velocity Max + Drag (packed)
        data.u_velMaxDrag = glm::vec4(velMax, drag);

        // 4. Gravity + colorSensitivity
        data.u_gravity = glm::vec4(gravity, colorSensitivity);

        // 5. Emitter Pos + Shape (Shape přetypujeme na float)
        data.u_emitterPosShape = glm::vec4(emitterPos, static_cast<float>(spawnShape));

        // 6. Emitter Size + Radius + Offset
        data.u_emitterSizeRadius = glm::vec4(emitterSize, emitterRadius, emitterYOffset);

        // 7. Spawn Area Settings
        data.u_spawnArea = glm::vec4(minRadius, maxRadius, spawnHeight, spawnPerFrame);

        // 8. Turbulence + Time + RespawnMode
        data.u_turbulenceTime = glm::vec4(turbulence, timeOffset, static_cast<float>(respawnMode));

        // 9. Colors
        data.u_colorStart = colorStart;
        data.u_colorEnd = colorEnd;

        return data;
    }
} // Material
