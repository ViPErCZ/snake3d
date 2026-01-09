#ifndef SNAKE3_STANDARDMATERIAL_H
#define SNAKE3_STANDARDMATERIAL_H

#include <memory>
#include "BaseMaterial.h"
#include "IAlbedoMaterial.h"
#include "../../../Lights/DirectionalLight.h"
#include "../../../Lights/PointLight.h"
#include "../../../Lights/SpotLight.h"
#include "../../../Manager/ShaderManager.h"
#include "../../../Manager/TextureManager.h"
#include "../../../Tools/Timer.h"
#include "../../../Tools/WorldEnvironment.h"

using namespace Manager;
using namespace Lights;
using namespace Tools;
using namespace std;

namespace Material {
    class StandardMaterial : public BaseMaterial, public IAlbedoMaterial {
    public:
        explicit StandardMaterial(shared_ptr<ShaderManager> baseShader,
                                  shared_ptr<ShaderManager> shadowDepthShader,
                                  const shared_ptr<WorldEnvironment> &worldEnv = nullptr);

        ~StandardMaterial() override;

        void setAlbedo(const shared_ptr<TextureManager> &albedo);

        void setNormal(const shared_ptr<TextureManager> &normal);

        void setSpecular(const shared_ptr<TextureManager> &specular);

        void setShadow(const shared_ptr<TextureManager> &shadow);

        [[nodiscard]] shared_ptr<TextureManager> getAlbedo() const override;

        [[nodiscard]] shared_ptr<TextureManager> getSpecular() const;

        [[nodiscard]] shared_ptr<TextureManager> getNormal() const;

        [[nodiscard]] shared_ptr<TextureManager> getShadow() const;

        void setNormalEnabled(bool normal_enabled);

        virtual void bind(const glm::vec3 &posView, const glm::mat4 &view, const glm::mat4 &projection,
                          const glm::mat4 &model, bool shadows) const;

        void bindShadow(const glm::mat4 &model) const;

        virtual void unbind() const;

        [[nodiscard]] glm::vec3 getColor() const;

        void setColor(const glm::vec3 &color);

        [[nodiscard]] bool isShadowEnabled() const;

        void setShadow(bool shadow_enabled);

        void setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light);

        void addSpotLight(const shared_ptr<SpotLight> &spot_light);

        void addPointLight(const shared_ptr<PointLight> &point_light);

        void setShininess(float shininess);

        [[nodiscard]] shared_ptr<TextureManager> getRoughness() const;

        void setRoughness(const shared_ptr<TextureManager> &roughness);

        void setAlpha(float alpha);

        [[nodiscard]] shared_ptr<TextureManager> getMetalness() const;

        void setMetalness(const shared_ptr<TextureManager> &metalness);

        void setEnvironmentMap(const shared_ptr<TextureManager> &environment_map);

        void setAoMap(const shared_ptr<TextureManager> &ao_map);

        void set_uv_scale(const glm::vec2 &uv_scale);

        void set_uv_offset(const glm::vec2 &uv_offset);

        void setAmbientLightColorIntensity(float intensity);

        [[nodiscard]] std::shared_ptr<BaseMaterial> clone() const override;

        void bindUseBones(bool useBones) const;

        void bindBonesMatrices(int index, const glm::mat4 &matrice) const;

        void bindModel(const glm::mat4 &model) const;

        void bindShadowModel(const glm::mat4 &model) const;

    protected:
        shared_ptr<Timer> timer;
        shared_ptr<TextureManager> albedo;
        shared_ptr<TextureManager> normal;
        shared_ptr<TextureManager> specular;
        shared_ptr<TextureManager> roughness;
        shared_ptr<TextureManager> metalness;
        shared_ptr<TextureManager> shadow;
        shared_ptr<TextureManager> aoMap;
        shared_ptr<TextureManager> environmentMap;
        shared_ptr<ShaderManager> shader;
        shared_ptr<ShaderManager> shadowDepthShader;
        shared_ptr<WorldEnvironment> worldEnvironment;
        shared_ptr<DirectionalLight> directionalLight;
        vector<shared_ptr<SpotLight> > spotLights;
        vector<shared_ptr<PointLight> > pointLights;
        shared_ptr<glm::vec3> color = nullptr;
        glm::vec2 UVScale = {1, 1};
        glm::vec2 UVOffset = {0, 0};
        bool normal_enabled = false;
        bool shadowsEnabled = false;
        bool unShaded = false;
        float shininess = 32.0f;
        float alpha = 1.0f;
        float ambientLightColorIntensity = 0.05;
    };
}

#endif //SNAKE3_STANDARDMATERIAL_H
