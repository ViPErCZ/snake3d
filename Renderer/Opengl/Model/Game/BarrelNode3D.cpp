#include "BarrelNode3D.h"

#include "../../Material/MaterialBuilder.h"
#include "../../Material/Feature/LightingFeature.h"
#include "../../Material/Feature/ShadowFeature.h"
#include "../Standard/ArrayMesh.h"

namespace Model {
    BarrelNode3D::BarrelNode3D(const shared_ptr<ContextState> &contextState,
                               const shared_ptr<ResourceManager> &resourceManager) : MeshNode3D(
        contextState, nullptr, resourceManager) {
    }

    void BarrelNode3D::init() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto barrel = make_shared<ArrayMesh>(shader);
        barrel->fromMesh(resourceManager->getModel("barrel_0"));
        setPosition({0.6, 0.0f, -7.2f});
        setScale({0.13888889, 0.13888889, 0.13888889});
        setRotationX(90);

        albedoFeature = make_shared<Feature::AlbedoFeature>(nullptr);
        albedoFeature->setAmbientIntensity(2.5f);
        normalFeature = make_shared<Feature::NormalMapFeature>(nullptr);
        material = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight,
                                                       pointLights,
                                                       std::vector<std::shared_ptr<Lights::SpotLight>>{}))
            .with(make_shared<Feature::ShadowFeature>(resourceManager->getTexture("depth"), shadowsShader))
            .with(normalFeature)
            .with(albedoFeature)
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        material->setBlending(Blending::Opaque);
        barrel->setMaterial(material);

        mesh = barrel;

        setVisible(false);
    }

    void BarrelNode3D::update(const float dt, const uint64_t frameId) {
        MeshNode3D::update(dt, frameId);

        if (!initialized) {
            if (albedoFeature->getAlbedo() == nullptr) {
                const auto barrelAlbedoTexture = mesh->getMesh()->getTextures()[0].texture;
                barrelAlbedoTexture->lazyLoad(true);
                albedoFeature->setAlbedo(barrelAlbedoTexture);
            } else if (normalFeature->getTexture() == nullptr) {
                const auto barrelNormalTexture = mesh->getMesh()->getTextures()[1].texture;
                barrelNormalTexture->lazyLoad(true);
                normalFeature->setTexture(barrelNormalTexture);
            }

            if (albedoFeature->getAlbedo() != nullptr && normalFeature->getTexture() != nullptr) {
                initialized = true;
                setVisible(true);
            }
        }
    }
} // Model
