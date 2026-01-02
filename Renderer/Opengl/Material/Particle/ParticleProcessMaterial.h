#ifndef SNAKE3_PARTICLEPROCESSMATERIAL_H
#define SNAKE3_PARTICLEPROCESSMATERIAL_H

#include <memory>

#include "BaseProcessMaterial.h"
#include "../../../../Manager/ShaderManager.h"

using namespace Manager;
using namespace std;

namespace Material {
    class ParticleProcessMaterial : public BaseProcessMaterial {
    public:
        explicit ParticleProcessMaterial(const shared_ptr<ResourceManager> &resource_manager);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection, const glm::mat4 &model,
                  bool shadows) const override;

        void update(int maxParticles, float timeAccum, float timeOffset, float stepDt) override;

    private:
        shared_ptr<ShaderManager> update_shader;
        shared_ptr<ShaderManager> render_shader;
        shared_ptr<ShaderManager> render_texture_shader;
    };
} // Material

#endif //SNAKE3_PARTICLEPROCESSMATERIAL_H
