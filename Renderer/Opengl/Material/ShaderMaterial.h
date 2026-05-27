#ifndef SNAKE3_SHADERMATERIAL_H
#define SNAKE3_SHADERMATERIAL_H

#include <map>
#include <memory>
#include <variant>
#include <vector>

#include <glm/glm.hpp>

#include "BaseMaterial.h"
#include "IUniform.h"
#include "../../../Lights/DirectionalLight.h"
#include "../../../Lights/PointLight.h"
#include "../../../Lights/SpotLight.h"
#include "../../../Manager/MaterialUbo.h"
#include "../../../Manager/ShaderProgram.h"
#include "../../../Manager/TextureManager.h"

namespace Material {

    using Manager::ShaderProgram;
    using Manager::TextureManager;

    using UniformValue = std::variant<
            bool,
            int,
            float,
            glm::vec2,
            glm::vec3,
            glm::vec4,
            glm::mat2,
            glm::mat3,
            glm::mat4,
            std::shared_ptr<IUniform>
        >;

    class ShaderMaterial final : public BaseMaterial {
    public:
        explicit ShaderMaterial(std::shared_ptr<ShaderProgram> baseShader,
                                std::shared_ptr<ShaderProgram> shadowDepthShader = nullptr);

        ~ShaderMaterial() override = default;

        void setUniform(const std::string &name, const UniformValue &value);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::mat4 &model, bool shadows) const;
        void unbind() const;
        void bindShadow(const glm::mat4 &model) const;

        [[nodiscard]] bool isShadowEnabled() const { return shadowsEnabled; }
        [[nodiscard]] std::shared_ptr<ShaderProgram> getShader() const           { return shader; }
        [[nodiscard]] std::shared_ptr<ShaderProgram> getShadowDepthShader() const{ return shadowDepthShader; }

        [[nodiscard]] std::shared_ptr<BaseMaterial> clone() const override;

        // Textures (shared resources, owned externally).
        void setAlbedo(const std::shared_ptr<TextureManager> &tex)   { albedo = tex; }
        void setNormal(const std::shared_ptr<TextureManager> &tex)   { normal = tex; }
        void setSpecular(const std::shared_ptr<TextureManager> &tex) { specular = tex; }
        void setMetalness(const std::shared_ptr<TextureManager> &tex){ metalness = tex; }
        void setRoughness(const std::shared_ptr<TextureManager> &tex){ roughness = tex; }
        void setShadow(const std::shared_ptr<TextureManager> &tex)   { shadow = tex; shadowsEnabled = true; }

        [[nodiscard]] std::shared_ptr<TextureManager> getAlbedo() const   { return albedo; }
        [[nodiscard]] std::shared_ptr<TextureManager> getNormal() const   { return normal; }
        [[nodiscard]] std::shared_ptr<TextureManager> getSpecular() const { return specular; }
        [[nodiscard]] std::shared_ptr<TextureManager> getMetalness() const{ return metalness; }
        [[nodiscard]] std::shared_ptr<TextureManager> getRoughness() const{ return roughness; }
        [[nodiscard]] std::shared_ptr<TextureManager> getShadow() const   { return shadow; }

        // Lighting (mirrors what StandardMaterial::bind used to do).
        void setDirectionalLight(const std::shared_ptr<Lights::DirectionalLight> &d) { directionalLight = d; }
        void setSpotLights(const std::vector<std::shared_ptr<Lights::SpotLight>> &s)      { spotLights = s; }
        void setPointLights(const std::vector<std::shared_ptr<Lights::PointLight>> &p)    { pointLights = p; }
        void addSpotLight(const std::shared_ptr<Lights::SpotLight> &s)               { spotLights.push_back(s); }
        void addPointLight(const std::shared_ptr<Lights::PointLight> &p)             { pointLights.push_back(p); }

        // Color / scalar params used by some legacy custom shaders.
        void setColor(const glm::vec3 &c)        { color = std::make_shared<glm::vec3>(c); }
        [[nodiscard]] bool hasColor() const      { return color != nullptr; }
        [[nodiscard]] glm::vec3 getColor() const { return color ? *color : glm::vec3(0.0f); }
        void setAlpha(const float a)             { alpha = a; }

    protected:
        std::shared_ptr<ShaderProgram> shader;
        std::shared_ptr<ShaderProgram> shadowDepthShader;
        std::map<std::string, UniformValue> uniforms;

        std::shared_ptr<TextureManager> albedo;
        std::shared_ptr<TextureManager> normal;
        std::shared_ptr<TextureManager> specular;
        std::shared_ptr<TextureManager> metalness;
        std::shared_ptr<TextureManager> roughness;
        std::shared_ptr<TextureManager> shadow;

        std::shared_ptr<Lights::DirectionalLight> directionalLight;
        std::vector<std::shared_ptr<Lights::SpotLight>> spotLights;
        std::vector<std::shared_ptr<Lights::PointLight>> pointLights;

        std::shared_ptr<glm::vec3> color;
        float alpha = 1.0f;
        bool shadowsEnabled = false;

        // Per-material UBO (slot 1) populated from uniforms map at bind time.
        // Shared lights.glsl helpers (CalcDirLight et al.) read `material_*`
        // fields from this block, so ShaderMaterial-driven shaders (respawn.fs,
        // explosion.fs) get a deterministic UBO state instead of stale data
        // from whatever MaterialInstance was bound last.
        mutable Manager::MaterialUbo materialUbo;
        mutable Manager::MaterialDataStd140 cpu{};
    };
} // Material

#endif //SNAKE3_SHADERMATERIAL_H
