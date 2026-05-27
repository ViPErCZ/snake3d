#include "ShaderMaterial.h"

#include <utility>
#include <variant>

namespace Material {
    ShaderMaterial::ShaderMaterial(shared_ptr<ShaderProgram> baseShader,
                                   shared_ptr<ShaderProgram> shadowDepthShader)
        : shader(std::move(baseShader)),
          shadowDepthShader(std::move(shadowDepthShader)) {
    }

    void ShaderMaterial::setUniform(const string &name, const UniformValue &value) {
        uniforms[name] = value;
    }

    namespace {
        // Read a typed value from the uniforms map; returns fallback if the
        // key is missing or holds a different variant alternative.
        template <typename T>
        T readUniformOr(const std::map<std::string, UniformValue>& uniforms,
                        const std::string& key, T fallback) {
            const auto it = uniforms.find(key);
            if (it == uniforms.end()) return fallback;
            if (const auto* v = std::get_if<T>(&it->second)) return *v;
            return fallback;
        }
    }

    void ShaderMaterial::bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
        const glm::mat4 &model, const bool shadows) const {
        shader->use();

        // Populate MaterialData UBO so lights.glsl's `material_*` reads are
        // deterministic when ShaderMaterial-driven shaders (respawn.fs,
        // explosion.fs) call CalcDirLight. Defaults match legacy uniform
        // defaults; uniforms map overrides what the owner set explicitly.
        cpu.material_alpha = alpha;
        cpu.material_ambientLightColor = glm::vec3(1.0f);
        cpu.material_ambientLightColorIntensity = 1.0f;
        cpu.material_useMaterial =
            readUniformOr<bool>(uniforms, "useMaterial", false) ? 1 : 0;
        cpu.material_hasAlbedoTexture =
            readUniformOr<bool>(uniforms, "hasAlbedoTexture", false) ? 1 : 0;
        cpu.material_overrideColorMesh = 0;
        materialUbo.upload(cpu);
        materialUbo.bind();
        if (shader->hasUniform("view") && uniforms.contains("view") == false) {
            shader->setMat4("view", view);
        }
        shader->setMat4("projection", projection);
        shader->setMat4("model", model);
        if (shader->hasUniform("expansion") && uniforms.contains("expansion") == false) {
            shader->setFloat("expansion", 1.0f);
        }
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

        // POINT LIGHT
        // --------------------------------
        shader->setInt("numPointLights", static_cast<int>(pointLights.size()));
        int index = 0;
        for (const auto &pointLight: pointLights) {
            pointLight->bind(shader.get(), index);
            index++;
        }
        // --------------------------------
        // END POINT LIGHT

        // SPOT LIGHT
        // --------------------------------
        shader->setInt("numSpotLights", static_cast<int>(spotLights.size()));
        index = 0;
        for (const auto &spotLight: spotLights) {
            spotLight->bind(shader.get(), index);
            index++;
        }
        // -----------------------------------------------
        // END SPOT LIGHT

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

    void ShaderMaterial::unbind() const {
        // Conservative cleanup: legacy StandardMaterial::unbind released
        // texture slots 0-7. For ShaderMaterial those textures might not
        // even be bound by the custom shader, so leaving things as-is is
        // safer. If a specific shader needs slot resets, do them through
        // the uniforms map / a per-feature mechanism later.
    }

    void ShaderMaterial::bindShadow(const glm::mat4 &model) const {
        if (shadowDepthShader) {
            shadowDepthShader->use();
            shadowDepthShader->setMat4("model", model);
        }
    }

    std::shared_ptr<BaseMaterial> ShaderMaterial::clone() const {
        auto cloned = std::make_shared<ShaderMaterial>(shader, shadowDepthShader);

        // Shared resources (textures, lights) - intentionally copied by
        // shared_ptr value; they're immutable engine resources.
        cloned->albedo = albedo;
        cloned->normal = normal;
        cloned->specular = specular;
        cloned->metalness = metalness;
        cloned->roughness = roughness;
        cloned->shadow = shadow;
        cloned->shadowsEnabled = shadowsEnabled;
        cloned->directionalLight = directionalLight;
        cloned->spotLights = spotLights;
        cloned->pointLights = pointLights;
        if (color) cloned->color = std::make_shared<glm::vec3>(*color);
        cloned->alpha = alpha;

        for (const auto& [name, value] : uniforms) {
            cloned->setUniform(name, std::visit([]<typename T0>(T0&& v) -> UniformValue {
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
