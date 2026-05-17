#ifndef SNAKE3_PLANEMATERIAL_H
#define SNAKE3_PLANEMATERIAL_H

#include "PlanarReflectionMaterial.h"

namespace Material {
    class PlaneMaterial final : public PlanarReflectionMaterial {
    public:
        explicit PlaneMaterial(const shared_ptr<ShaderManager> &baseShader,
                               const shared_ptr<ShaderManager> &shadowDepthShader,
                               const shared_ptr<WorldEnvironment> &worldEnv = nullptr);

        void setHoleMap(const shared_ptr<TextureManager> &holeMap);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::mat4 &model, bool shadows) const override;

        void unbind() const override;

    protected:
        shared_ptr<TextureManager> holeMap;
    };
}

#endif //SNAKE3_PLANEMATERIAL_H
