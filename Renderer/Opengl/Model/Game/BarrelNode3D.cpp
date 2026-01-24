#include "BarrelNode3D.h"

#include "../Standard/ArrayMesh.h"

namespace Model {
    BarrelNode3D::BarrelNode3D(const shared_ptr<ContextState> &contextState,
        const shared_ptr<ResourceManager> &resourceManager) : MeshNode3D(contextState, nullptr, resourceManager) {
    }

    void BarrelNode3D::init() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto barrel = make_shared<ArrayMesh>(shader);
        barrel->fromMesh(resourceManager->getModel("barrel_0"));
        setPosition({0.6,  0.0f, -7.2f});
        setScale({0.13888889, 0.13888889, 0.13888889});
        setRotationX(90);
        const auto barrelMaterial = make_shared<StandardMaterial>(shader, shadowsShader);
        barrelMaterial->setNormalEnabled(true);
        barrelMaterial->setDirectionalLight(directionalLight);
        barrelMaterial->setBlending(Blending::Opaque);
        barrelMaterial->setShadow(resourceManager->getTexture("depth"));
        const auto barrelAlbedoTexture = barrel->getMesh()->getTextures()[0].texture;
        barrelAlbedoTexture->lazyLoad(true);
        barrelMaterial->setAlbedo(barrelAlbedoTexture);
        const auto barrelNormalTexture = barrel->getMesh()->getTextures()[1].texture;
        barrelNormalTexture->lazyLoad(true);
        barrelMaterial->setPointLights(pointLights);
        barrelMaterial->setAmbientLightColorIntensity(2.5f);
        barrel->setMaterial(barrelMaterial);

        mesh = barrel;
    }
} // Model