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

        const auto streetLampMesh2 = make_shared<ArrayMesh>(shader);
        streetLampMesh2->fromMesh(resourceManager->getModel("streetlamp_1"));
        streetLampMesh2->setDepthWrite(false);

        const auto streetLampMesh3 = make_shared<ArrayMesh>(shader);
        streetLampMesh3->fromMesh(resourceManager->getModel("streetlamp_2"));

        const auto streetLamp2 = make_shared<MeshNode3D>(contextState, streetLampMesh2, resourceManager);
        const auto streetLamp3 = make_shared<MeshNode3D>(contextState, streetLampMesh3, resourceManager);
        setPosition({0.0,  0.0f, -7.2f});
        setScale({0.13888889, 0.13888889, 0.13888889});
        setRotationX(90);
        streetLamp2->addNode(streetLamp3);

        mesh = streetLampMesh1;
        addNode(streetLamp2);

        const auto streetLampMaterial = make_shared<StandardMaterial>(shader, shadowsShader);
        streetLampMaterial->setNormalEnabled(true);
        streetLampMaterial->setDirectionalLight(directionalLight);
        streetLampMaterial->setBlending(Blending::Opaque);
        streetLampMaterial->setShadow(resourceManager->getTexture("depth"));
        const auto albedoTexture = mesh->getMesh()->getTextures()[0].texture;
        albedoTexture->lazyLoad(true);
        streetLampMaterial->setAlbedo(albedoTexture);
        const auto normalTexture = mesh->getMesh()->getTextures()[1].texture;
        normalTexture->lazyLoad(true);
        const auto pbrTexture = mesh->getMesh()->getTextures()[2].texture;
        pbrTexture->lazyLoad(true);
        streetLampMaterial->setRoughness(pbrTexture);
        streetLampMaterial->setMetalness(pbrTexture);
        streetLampMaterial->setAoMap(pbrTexture);
        streetLampMaterial->setEnvironmentMap(resourceManager->getTexture("skybox"));
        streetLampMaterial->setNormal(normalTexture);
        streetLampMaterial->setPointLights(pointLights);
        streetLampMaterial->setAmbientLightColorIntensity(2.5f);
        mesh->setMaterial(streetLampMaterial);

        const auto streetLampMaterial2 = make_shared<StandardMaterial>(shader, shadowsShader);
        streetLampMaterial2->setDirectionalLight(directionalLight);
        streetLampMaterial2->setBlending(Blending::Additive);
        streetLampMaterial2->setNormalEnabled(true);
        streetLampMaterial2->setShadow(resourceManager->getTexture("depth"));
        const auto albedoTexture2 = streetLampMesh2->getMesh()->getTextures()[0].texture;
        albedoTexture2->lazyLoad(true);
        streetLampMaterial2->setAlbedo(albedoTexture2);
        streetLampMaterial2->setNormal(normalTexture);
        streetLampMaterial2->setPointLights(pointLights);
        streetLampMaterial2->setRoughness(pbrTexture);
        streetLampMaterial2->setMetalness(pbrTexture);
        streetLampMaterial2->setAoMap(pbrTexture);
        streetLampMaterial2->setAmbientLightColorIntensity(2);
        streetLampMesh2->setMaterial(streetLampMaterial2);

        const auto streetLampMaterial3 = make_shared<StandardMaterial>(shader, shadowsShader);
        streetLampMaterial3->setDirectionalLight(directionalLight);
        streetLampMaterial3->setBlending(Blending::Opaque);
        streetLampMaterial3->setNormalEnabled(true);
        streetLampMaterial3->setColor({0.98 * 200, 0.99 * 200, 0.007 * 200});
        streetLampMaterial3->setNormal(streetLampMesh3->getMesh()->getTextures()[1].texture);
        streetLampMaterial3->setPointLights(pointLights);
        streetLampMesh3->setMaterial(streetLampMaterial3);
    }
} // Model