#include "StreetLampNode3D.h"

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

        material1 = make_shared<StandardMaterial>(shader, shadowsShader);
        material1->setNormalEnabled(true);
        material1->setDirectionalLight(directionalLight);
        material1->setBlending(Blending::Opaque);
        material1->setShadow(resourceManager->getTexture("depth"));

        material1->setEnvironmentMap(resourceManager->getTexture("skybox"));
        material1->setPointLights(pointLights);
        material1->setAmbientLightColorIntensity(2.5f);
        mesh->setMaterial(material1);

        material2 = make_shared<StandardMaterial>(shader, shadowsShader);
        material2->setDirectionalLight(directionalLight);
        material2->setBlending(Blending::Additive);
        material2->setNormalEnabled(true);
        material2->setShadow(resourceManager->getTexture("depth"));

        material2->setPointLights(pointLights);
        material2->setAmbientLightColorIntensity(2);
        mesh2->setMaterial(material2);

        material3 = make_shared<StandardMaterial>(shader, shadowsShader);
        material3->setDirectionalLight(directionalLight);
        material3->setBlending(Blending::Opaque);
        material3->setNormalEnabled(true);
        material3->setColor({0.98 * 200, 0.99 * 200, 0.007 * 200});
        material3->setPointLights(pointLights);
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

                material1->setAlbedo(albedoTexture);
                material1->setNormal(normalTexture);
                material1->setRoughness(pbrTexture);
                material1->setMetalness(pbrTexture);
                material1->setAoMap(pbrTexture);

                const auto albedoTexture2 = resourceManager->getTexture("streetlamp_" + mesh2->getMesh()->getTextures()[0].path);
                material2->setNormal(normalTexture);
                material2->setAlbedo(albedoTexture2);
                material2->setRoughness(pbrTexture);
                material2->setMetalness(pbrTexture);
                material2->setAoMap(pbrTexture);

                material3->setNormal(normalTexture);
            }
        }
    }
} // Model