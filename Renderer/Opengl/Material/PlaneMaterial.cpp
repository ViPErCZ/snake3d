#include "PlaneMaterial.h"

namespace Material {
    PlaneMaterial::PlaneMaterial(const shared_ptr<ShaderManager> &baseShader,
                                 const shared_ptr<ShaderManager> &shadowDepthShader,
                                 const shared_ptr<WorldEnvironment> &worldEnv)
        : PlanarReflectionMaterial(baseShader, shadowDepthShader, worldEnv) {
    }

    void PlaneMaterial::setHoleMap(const shared_ptr<TextureManager> &holeMap) {
        this->holeMap = holeMap;
    }

    void PlaneMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                             const glm::mat4 &model, const bool shadows) const {
        PlanarReflectionMaterial::bind(posView, view, projection, model, shadows);

        if (holeMap && holeMap->hasTexture()) {
            shader->setBool("hasHoleMap", true);
            shader->setInt("holeMap", 8);
            holeMap->bind(8);
        } else {
            shader->setBool("hasHoleMap", false);
        }
    }

    void PlaneMaterial::unbind() const {
        PlanarReflectionMaterial::unbind();
        if (holeMap) {
            holeMap->unbind(8);
        }
    }
}
