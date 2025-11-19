#include "ShaderMaterial.h"

#include <utility>
#include <variant>

namespace Material {
    ShaderMaterial::ShaderMaterial(shared_ptr<ShaderManager> baseShader,
                                   shared_ptr<ShaderManager> shadowDepthShader,
                                   const shared_ptr<WorldEnvironment> &worldEnv) : StandardMaterial(
        std::move(baseShader), std::move(shadowDepthShader), worldEnv) {
    }

    ShaderMaterial::~ShaderMaterial() = default;

    void ShaderMaterial::addUniform(const string &name, const UniformValue &value) {
        uniforms[name] = value;
    }

    void ShaderMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
        const glm::mat4 &model, const bool shadows) const {
        shader->use();
        if (shader->hasUniform("view")) {
            shader->setMat4("view", view);
        }
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);
        if (shader->hasUniform("viewPos")) {
            shader->setVec3("viewPos", posView);
        }
        if (shader->hasUniform("shadows")) {
            shader->setBool("shadows", shadows);
        }

        if (directionalLight) {
            directionalLight->bind(shader.get());
            shader->setBool("directionLightEnable", true);
        }

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

    std::shared_ptr<BaseMaterial> ShaderMaterial::clone() const {
        auto cloned = std::make_shared<ShaderMaterial>(
            shader, shadowDepthShader, worldEnvironment
        );

        for (const auto& [name, value] : uniforms) {
            cloned->addUniform(name, std::visit([]<typename T0>(T0&& v) -> UniformValue {
                using T = std::decay_t<T0>;
                if constexpr (std::is_base_of_v<IUniform, T>) {
                    return v.clone();
                } else if constexpr (std::is_same_v<T, std::shared_ptr<IUniform>>) {
                    return v ? v->clone() : nullptr;
                } else {
                    return v;
                }
            }, value));
        }

        return cloned;
    }
} // Material
