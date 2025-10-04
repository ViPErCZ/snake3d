#include "ShaderMaterial.h"

#include <utility>
#include <variant>

namespace Material {
    ShaderMaterial::ShaderMaterial(shared_ptr<ShaderManager> baseShader, shared_ptr<ShaderManager> shadowDepthShader,
                                   const shared_ptr<WorldEnvironment> &worldEnv) : StandardMaterial(
        std::move(baseShader), std::move(shadowDepthShader), worldEnv) {
    }

    ShaderMaterial::~ShaderMaterial() = default;

    void ShaderMaterial::addUniform(const string &name, const UniformValue &value) {
        uniforms.emplace(name, value);
    }

    void ShaderMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
        const glm::mat4 &model) const {
        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);
        shader->setVec3("viewPos", posView);

        for (auto& [name, value] : uniforms) {
            std::visit([&]<typename T0>(T0&& arg) {
                using T = std::decay_t<T0>;
                if constexpr (std::is_same_v<T, std::shared_ptr<IUniform>>) {
                    if (arg) arg->bind(shader, name);
                } else {
                    shader->setUniform(name, arg);
                }
            }, value);
        }
    }
} // Material
