#ifndef SNAKE3_PARTICLEPROCESSMATERIAL_H
#define SNAKE3_PARTICLEPROCESSMATERIAL_H

#include <memory>

#include "BaseProcessMaterial.h"

using namespace Manager;
using namespace std;

namespace Material {
    class ParticleProcessMaterial : public BaseProcessMaterial {
        struct ParticleDataBuffer {
            glm::vec4 emitterPos;   // x,y,z + spawnShape
            glm::vec4 emitterSize;  // x,y,z + padding
            glm::vec4 gravity;      // x,y,z + drag
            glm::vec4 velRange;     // minSpd, maxSpd, turb, sticky
            glm::vec4 lifeParams;   // minLife, maxLife, padding, padding
            glm::vec4 randoms;      // time, dt, seed_iter, padding
        };
    public:
        explicit ParticleProcessMaterial(const shared_ptr<ResourceManager> &resource_manager);

        void bind(shared_ptr<ShaderManager> shader, const glm::vec3 &posView, const glm::mat4 &view,
                  const glm::mat4 &projection, const glm::mat4 &model, bool shadows) const override;

        void update(shared_ptr<ShaderManager> shader, int maxParticles, float timeAccum, float timeOffset,
                    float stepDt) override;
    };
} // Material

#endif //SNAKE3_PARTICLEPROCESSMATERIAL_H
