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
#include "../../../Manager/ShaderProgram.h"
#include "../../../Manager/TextureManager.h"

using namespace Manager;
using namespace Lights;
using namespace std;

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

    class ShaderMaterial final : public BaseMaterial {
    public:
        explicit ShaderMaterial(shared_ptr<ShaderProgram> baseShader,
                                shared_ptr<ShaderProgram> shadowDepthShader = nullptr);

        ~ShaderMaterial() override = default;

        void setUniform(const string &name, const UniformValue &value);

        void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                  const glm::mat4 &model, bool shadows) const;
        void unbind() const;
        void bindShadow(const glm::mat4 &model) const;

        [[nodiscard]] bool isShadowEnabled() const { return shadowsEnabled; }
        [[nodiscard]] shared_ptr<ShaderProgram> getShader() const           { return shader; }
        [[nodiscard]] shared_ptr<ShaderProgram> getShadowDepthShader() const{ return shadowDepthShader; }

        [[nodiscard]] std::shared_ptr<BaseMaterial> clone() const override;

        // Textures (shared resources, owned externally).
        void setAlbedo(const shared_ptr<TextureManager> &tex)   { albedo = tex; }
        void setNormal(const shared_ptr<TextureManager> &tex)   { normal = tex; }
        void setSpecular(const shared_ptr<TextureManager> &tex) { specular = tex; }
        void setMetalness(const shared_ptr<TextureManager> &tex){ metalness = tex; }
        void setRoughness(const shared_ptr<TextureManager> &tex){ roughness = tex; }
        void setShadow(const shared_ptr<TextureManager> &tex)   { shadow = tex; shadowsEnabled = true; }

        [[nodiscard]] shared_ptr<TextureManager> getAlbedo() const   { return albedo; }
        [[nodiscard]] shared_ptr<TextureManager> getNormal() const   { return normal; }
        [[nodiscard]] shared_ptr<TextureManager> getSpecular() const { return specular; }
        [[nodiscard]] shared_ptr<TextureManager> getMetalness() const{ return metalness; }
        [[nodiscard]] shared_ptr<TextureManager> getRoughness() const{ return roughness; }
        [[nodiscard]] shared_ptr<TextureManager> getShadow() const   { return shadow; }

        // Lighting (mirrors what StandardMaterial::bind used to do).
        void setDirectionalLight(const shared_ptr<DirectionalLight> &d) { directionalLight = d; }
        void setSpotLights(const vector<shared_ptr<SpotLight>> &s)      { spotLights = s; }
        void setPointLights(const vector<shared_ptr<PointLight>> &p)    { pointLights = p; }
        void addSpotLight(const shared_ptr<SpotLight> &s)               { spotLights.push_back(s); }
        void addPointLight(const shared_ptr<PointLight> &p)             { pointLights.push_back(p); }

        // Color / scalar params used by some legacy custom shaders.
        void setColor(const glm::vec3 &c)        { color = make_shared<glm::vec3>(c); }
        [[nodiscard]] bool hasColor() const      { return color != nullptr; }
        [[nodiscard]] glm::vec3 getColor() const { return color ? *color : glm::vec3(0.0f); }
        void setAlpha(const float a)             { alpha = a; }

    protected:
        shared_ptr<ShaderProgram> shader;
        shared_ptr<ShaderProgram> shadowDepthShader;
        std::map<std::string, UniformValue> uniforms;

        shared_ptr<TextureManager> albedo;
        shared_ptr<TextureManager> normal;
        shared_ptr<TextureManager> specular;
        shared_ptr<TextureManager> metalness;
        shared_ptr<TextureManager> roughness;
        shared_ptr<TextureManager> shadow;

        shared_ptr<DirectionalLight> directionalLight;
        vector<shared_ptr<SpotLight>> spotLights;
        vector<shared_ptr<PointLight>> pointLights;

        shared_ptr<glm::vec3> color;
        float alpha = 1.0f;
        bool shadowsEnabled = false;
    };
} // Material

#endif //SNAKE3_SHADERMATERIAL_H
