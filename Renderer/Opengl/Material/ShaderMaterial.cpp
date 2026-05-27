#include "ShaderMaterial.h"

#include <utility>
#include <variant>

using namespace std;

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
        // D1.1c: gate the directional light block from the same UBO so the
        // setBool("directionLightEnable") call disappears. respawn.fs doesn't
        // gate its CalcDirLight call -- it always runs -- so the flag is more
        // for explosion.fs, but populating it costs nothing.
        cpu.material_directionLightEnable = directionalLight ? 1 : 0;
        // D1.1c-fix: per-material dirLight fields, mirrored from
        // LightingFeature's contract. Default-zero when no light is wired in
        // (matches MaterialDataStd140's struct defaults).
        if (directionalLight) {
            cpu.material_dirLight_direction = directionalLight->getDirection();
            cpu.material_dirLight_ambient   = directionalLight->getAmbient();
            cpu.material_dirLight_diffuse   = directionalLight->getDiffuse();
            cpu.material_dirLight_specular  = directionalLight->getSpecular();
        } else {
            cpu.material_dirLight_direction = glm::vec3(0.0f);
            cpu.material_dirLight_ambient   = glm::vec3(0.0f);
            cpu.material_dirLight_diffuse   = glm::vec3(0.0f);
            cpu.material_dirLight_specular  = glm::vec3(0.0f);
        }
        // D1.1d: per-material point + spot light arrays migrated into the
        // MaterialData UBO. Mirror the legacy SpotLight::bind transformations
        // (normalize(dir-pos), cos(radians(cutOff)), bool->int) so respawn.fs
        // / explosion.fs read the same values the per-program setUniform
        // path produced pre-D1.1d.
        {
            int pIdx = 0;
            for (const auto& pl : pointLights) {
                if (pl && pIdx < 8) {
                    auto& dst = cpu.material_pointLights[pIdx];
                    dst.position  = pl->getPosition();
                    dst.ambient   = pl->getAmbient();
                    dst.diffuse   = pl->getDiffuse();
                    dst.specular  = pl->getSpecular();
                    dst.constant  = pl->getConstant();
                    dst.linear    = pl->getLinear();
                    dst.quadratic = pl->getQuadratic();
                    ++pIdx;
                }
            }
            cpu.material_numPointLights = pIdx;

            int sIdx = 0;
            for (const auto& sl : spotLights) {
                if (sl && sIdx < 8) {
                    auto& dst = cpu.material_spotLights[sIdx];
                    dst.position    = sl->getPosition();
                    dst.direction   = glm::normalize(sl->getDirection() - sl->getPosition());
                    dst.ambient     = sl->getAmbient();
                    dst.diffuse     = sl->getDiffuse();
                    dst.specular    = sl->getSpecular();
                    dst.constant    = sl->getConstant();
                    dst.linear      = sl->getLinear();
                    dst.quadratic   = sl->getQuadratic();
                    dst.cutOff      = glm::cos(glm::radians(sl->getCutOff()));
                    dst.outerCutOff = glm::cos(glm::radians(sl->getOuterCutOff()));
                    dst.pulse       = sl->isPulse() ? 1 : 0;
                    ++sIdx;
                }
            }
            cpu.material_numSpotLights = sIdx;
        }
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
            // D1.1c: DirectionalLight::bind now only sets material.shininess
            // + sampler ints; pozice/směr/ambient/diffuse/specular jsou v
            // FrameData UBO. Gate `directionLightEnable` je v MaterialData
            // UBO (nahoře už nastaven na 1).
            directionalLight->bind(shader.get());
        }

        // D1.1d: per-light setUniform loops removed -- point + spot light
        // arrays were just written into the MaterialData UBO above. Shaders
        // (respawn.fs, explosion.fs) read material_pointLights[] /
        // material_spotLights[] / material_numPointLights / numSpotLights.

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
