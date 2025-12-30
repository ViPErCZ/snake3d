#include "PlanarReflectionMaterial.h"

namespace Material {
    PlanarReflectionMaterial::PlanarReflectionMaterial(const shared_ptr<ShaderManager> &baseShader,
                                                       const shared_ptr<ShaderManager> &shadowDepthShader,
                                                       const shared_ptr<WorldEnvironment> &worldEnv)
        : StandardMaterial(baseShader, shadowDepthShader, worldEnv), reflectionEnable(false) {
    }

    void PlanarReflectionMaterial::setReflectionTexture(const shared_ptr<TextureManager> &texture) {
        reflectionTexture = texture;
        reflectionEnable = true;
    }

    void PlanarReflectionMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                                        const glm::mat4 &model, bool shadows) const {
        StandardMaterial::bind(posView, view, projection, model, shadows);

        shader->use();

        if (reflectionTexture) {
            reflectionTexture->bind(20);
            shader->setInt("reflectionTexture", 20);
        }

        shader->setBool("reflectionEnable", reflectionEnable);

        // Pro hlavní scénu nastavíme clipPlane tak, aby nic neořezával
        shader->setVec4("clipPlane", glm::vec4(0, 0, 1, 1000.0f));
    }

    void PlanarReflectionMaterial::setReflectionEnabled(const bool reflection_enabled) {
        reflectionEnable = reflection_enabled;
    }

    void PlanarReflectionMaterial::unbind() const {
        StandardMaterial::unbind();
        if (reflectionTexture) {
            reflectionTexture->unbind(20);
        }
    }
}
