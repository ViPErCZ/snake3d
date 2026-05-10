#ifndef SNAKE3_PLANARREFLECTIONMATERIAL_H
#define SNAKE3_PLANARREFLECTIONMATERIAL_H

#include "StandardMaterial.h"

namespace Material {
    class PlanarReflectionMaterial : public StandardMaterial {
    public:
        explicit PlanarReflectionMaterial(const shared_ptr<ShaderManager> &baseShader,
                                          const shared_ptr<ShaderManager> &shadowDepthShader,
                                          const shared_ptr<WorldEnvironment> &worldEnv = nullptr);

        void setReflectionTexture(const shared_ptr<TextureManager> &texture);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::mat4 &model, bool shadows) const override;

        void setReflectionEnabled(bool reflection_enabled);

        void unbind() const override;

    protected:
        shared_ptr<TextureManager> reflectionTexture;
        bool reflectionEnable;
    };
}

#endif //SNAKE3_PLANARREFLECTIONMATERIAL_H
