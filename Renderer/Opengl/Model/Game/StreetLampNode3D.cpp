#include "StreetLampNode3D.h"

#include "../../Material/MaterialBuilder.h"
#include "../../Material/Feature/IblFeature.h"
#include "../../Material/Feature/LightingFeature.h"
#include "../../Material/Feature/ShadowFeature.h"
#include "../Standard/ArrayMesh.h"

namespace Model {
    StreetLampNode3D::StreetLampNode3D(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager) : MeshNode3D(contextState, nullptr, resourceManager) {
    }

    void StreetLampNode3D::init() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");

        const auto streetLampMesh1 = make_shared<ArrayMesh>(shader);
        streetLampMesh1->fromMesh(resourceManager->getModel("streetlamp_0"));

        mesh2 = make_shared<ArrayMesh>(shader);
        mesh2->fromMesh(resourceManager->getModel("streetlamp_1"));
        mesh2->setDepthWrite(false);

        mesh3 = make_shared<ArrayMesh>(shader);
        mesh3->fromMesh(resourceManager->getModel("streetlamp_2"));

        const auto streetLamp2 = make_shared<MeshNode3D>(contextState, mesh2, resourceManager);
        const auto streetLamp3 = make_shared<MeshNode3D>(contextState, mesh3, resourceManager);
        setPosition({0.0,  0.0f, -7.2f});
        setScale({0.13888889, 0.13888889, 0.13888889});
        setRotationX(90);
        streetLamp2->addNode(streetLamp3);

        mesh = streetLampMesh1;
        addNode(streetLamp2);

        const auto depthTex = resourceManager->getTexture("depth");
        const std::vector<std::shared_ptr<Lights::SpotLight>> emptySpots;

        // Material 1 - lamp post body. Albedo + normal + PBR + IBL.
        albedo1 = make_shared<Feature::AlbedoFeature>(nullptr);
        albedo1->setAmbientIntensity(2.5f);
        normal1 = make_shared<Feature::NormalMapFeature>(nullptr);
        pbr1 = make_shared<Feature::PbrFeature>(nullptr, nullptr, nullptr);
        auto ibl1 = make_shared<Feature::IblFeature>(resourceManager->getTexture("skybox"));
        material1 = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, emptySpots))
            .with(make_shared<Feature::ShadowFeature>(depthTex, shadowsShader))
            .with(normal1)
            .with(pbr1)
            .with(ibl1)
            .with(albedo1)
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        material1->setBlending(Blending::Opaque);
        mesh->setMaterial(material1);

        // Material 2 - lamp glass shade. Additive blending for bloom.
        albedo2 = make_shared<Feature::AlbedoFeature>(nullptr);
        albedo2->setAmbientIntensity(2.0f);
        normal2 = make_shared<Feature::NormalMapFeature>(nullptr);
        pbr2 = make_shared<Feature::PbrFeature>(nullptr, nullptr, nullptr);
        material2 = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, emptySpots))
            .with(make_shared<Feature::ShadowFeature>(depthTex, shadowsShader))
            .with(normal2)
            .with(pbr2)
            .with(albedo2)
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        material2->setBlending(Blending::Additive);
        mesh2->setMaterial(material2);

        // Material 3 - lamp bulb. HDR color drives bloom, no albedo texture.
        albedo3 = make_shared<Feature::AlbedoFeature>(nullptr);
        albedo3->setColor({0.98f * 200, 0.99f * 200, 0.007f * 200});
        normal3 = make_shared<Feature::NormalMapFeature>(nullptr);
        material3 = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, emptySpots))
            .with(normal3)
            .with(albedo3)
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        material3->setBlending(Blending::Opaque);
        mesh3->setMaterial(material3);
    }

    void StreetLampNode3D::update(const float dt, const uint64_t frameId) {
        MeshNode3D::update(dt, frameId);

        if (!initialized) {
            if (!resourceManager->hasTexture("streetlamp_" + mesh->getMesh()->getTextures()[0].path)) {
                const auto albedoTexture = mesh->getMesh()->getTextures()[0].texture;
                albedoTexture->lazyLoad(true);
                resourceManager->addTexture("streetlamp_" + mesh->getMesh()->getTextures()[0].path, albedoTexture);
            } else if (!resourceManager->hasTexture("streetlamp_" + mesh->getMesh()->getTextures()[1].path)) {
                const auto normalTexture = mesh->getMesh()->getTextures()[1].texture;
                normalTexture->lazyLoad(true);
                resourceManager->addTexture("streetlamp_" + mesh->getMesh()->getTextures()[1].path, normalTexture);
            } else if (!resourceManager->hasTexture("streetlamp_" + mesh->getMesh()->getTextures()[2].path)) {
                const auto pbrTexture = mesh->getMesh()->getTextures()[2].texture;
                pbrTexture->lazyLoad(true);
                resourceManager->addTexture("streetlamp_" + mesh->getMesh()->getTextures()[2].path, pbrTexture);
            } else if (!resourceManager->hasTexture("streetlamp_" + mesh2->getMesh()->getTextures()[0].path)) {
                const auto albedoTexture = mesh2->getMesh()->getTextures()[0].texture;
                albedoTexture->lazyLoad(true);
                resourceManager->addTexture("streetlamp_" + mesh2->getMesh()->getTextures()[0].path, albedoTexture);
                initialized = true;
            }

            if (initialized) {
                const auto albedoTexture = resourceManager->getTexture("streetlamp_" + mesh->getMesh()->getTextures()[0].path); // 0
                const auto normalTexture = resourceManager->getTexture("streetlamp_" + mesh->getMesh()->getTextures()[1].path); // 2
                const auto pbrTexture = resourceManager->getTexture("streetlamp_" + mesh->getMesh()->getTextures()[2].path); // 1

                albedo1->setAlbedo(albedoTexture);
                normal1->setTexture(normalTexture);
                pbr1->setRoughness(pbrTexture);
                pbr1->setMetalness(pbrTexture);
                pbr1->setAoMap(pbrTexture);

                const auto albedoTexture2 = resourceManager->getTexture("streetlamp_" + mesh2->getMesh()->getTextures()[0].path);
                normal2->setTexture(normalTexture);
                albedo2->setAlbedo(albedoTexture2);
                pbr2->setRoughness(pbrTexture);
                pbr2->setMetalness(pbrTexture);
                pbr2->setAoMap(pbrTexture);

                normal3->setTexture(normalTexture);
            }
        }
    }
} // Model
