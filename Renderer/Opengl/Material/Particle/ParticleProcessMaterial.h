#ifndef SNAKE3_PARTICLEPROCESSMATERIAL_H
#define SNAKE3_PARTICLEPROCESSMATERIAL_H

#include <memory>

#include "BaseProcessMaterial.h"

namespace Material {
    using std::shared_ptr;

    class ParticleProcessMaterial : public BaseProcessMaterial {
        struct ParticleDataGPU {
            glm::vec4 u_lifeSizeStretch;    // x=lifeMin, y=lifeMax, z=sizeMin, w=sizeMax
            glm::vec4 u_velMinStretch;      // xyz=velMin, w=stretch
            glm::vec4 u_velMaxDrag;         // xyz=velMax, w=drag
            glm::vec4 u_gravity;            // xyz=gravity, w=colorSensitivity
            glm::vec4 u_emitterPosShape;    // xyz=pos, w=spawnShape (cast na float)
            glm::vec4 u_emitterSizeRadius;  // xy=size, z=radius, w=yOffset
            glm::vec4 u_spawnArea;          // x=minRadius, y=maxRadius, z=spawnHeight, w=spawnPerFrame
            glm::vec4 u_turbulenceTime;     // xy=turbulence, z=timeOffset, w=respawnMode
            glm::vec4 u_colorStart;         // rgba
            glm::vec4 u_colorEnd;           // rgba
        };
    public:
        ~ParticleProcessMaterial() override;

        explicit ParticleProcessMaterial(const shared_ptr<Manager::ResourceManager> &resource_manager);

        void bind(shared_ptr<Manager::ShaderProgram> shader) const override;

        void update(shared_ptr<Manager::ShaderProgram> shader, int maxParticles, float timeAccum, float timeOffset,
                    float stepDt) override;

    private:
        [[nodiscard]] ParticleDataGPU prepareUniformData() const;

        GLuint uboID{};
    };
} // Material

#endif //SNAKE3_PARTICLEPROCESSMATERIAL_H
