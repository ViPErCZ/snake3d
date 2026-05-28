#include "BarrelNode3D.h"

#include <snake3d/Renderer/Opengl/Material/MaterialBuilder.h>
#include <snake3d/Renderer/Opengl/Material/Feature/LightingFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/ShadowFeature.h>
#include <snake3d/Renderer/Opengl/Model/Standard/ArrayMesh.h>
#include <snake3d/Resource/MaterialLoader.h>

using namespace std;
using namespace Manager;
using namespace Tools;
using namespace Material;

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

        // D3.6: JSON-driven spec. Static features (normalMap + albedo bez textur)
        // přijdou z JSONu - lazy texture pattern v update() je staví dál nullptr-
        // checkem na albedoFeature/normalFeature. Členské pointery vytahujeme
        // ze spec.builder přes featuresView().
        auto spec = resourceManager->loadMaterial("Assets/Materials/barrel.json");
        for (const auto& f : spec.builder.featuresView()) {
            if (auto a = dynamic_pointer_cast<Feature::AlbedoFeature>(f))    albedoFeature = a;
            if (auto n = dynamic_pointer_cast<Feature::NormalMapFeature>(f)) normalFeature = n;
        }
        if (spec.hasLighting) {
            spec.builder.with(make_shared<Feature::LightingFeature>(
                directionalLight,
                pointLights,
                std::vector<std::shared_ptr<Lights::SpotLight>>{}));
        }
        if (spec.hasShadow) {
            spec.builder.with(make_shared<Feature::ShadowFeature>(
                resourceManager->getTexture("depth"), shadowsShader));
        }
        if (spec.hasFog) {
            spec.builder.with(resourceManager->getFogFeature());
        }
        material = spec.builder.build(*resourceManager->getShaderRegistry());
        material->setBlending(spec.blending);
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
