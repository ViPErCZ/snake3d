#ifndef SNAKE3_SHADERMATERIAL_H
#define SNAKE3_SHADERMATERIAL_H

#include <map>
#include "IUniform.h"
#include "StandardMaterial.h"

namespace Material {

    using UniformValue = variant<
            bool,
            int,
            float,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::mat2,
            glm::mat3,
            glm::mat4,
            shared_ptr<IUniform>
        >;

    class ShaderMaterial final : public StandardMaterial {
    public:
        explicit ShaderMaterial(shared_ptr<ShaderManager> baseShader,
                                shared_ptr<ShaderManager> shadowDepthShader,
                                const shared_ptr<WorldEnvironment> &worldEnv = nullptr);

        ~ShaderMaterial() override;

        void addUniform(const string &name, const UniformValue &value);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::mat4 &model, bool shadows) const override;

        [[nodiscard]] std::shared_ptr<BaseMaterial> clone() const override;

    protected:
        std::map<std::string, UniformValue> uniforms;
    };
} // Material

#endif //SNAKE3_SHADERMATERIAL_H
