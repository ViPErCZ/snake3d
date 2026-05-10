#include "BarrelNode3D.h"

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
        material = make_shared<StandardMaterial>(shader, shadowsShader);
        material->setDirectionalLight(directionalLight);
        material->setBlending(Blending::Opaque);
        material->setShadow(resourceManager->getTexture("depth"));
        material->setPointLights(pointLights);
        material->setAmbientLightColorIntensity(2.5f);
        barrel->setMaterial(material);

        mesh = barrel;

        setVisible(false);
    }

    void BarrelNode3D::update(const float dt, const uint64_t frameId) {
        MeshNode3D::update(dt, frameId);

        if (!initialized) {
            if (material->getAlbedo() == nullptr) {
                const auto barrelAlbedoTexture = mesh->getMesh()->getTextures()[0].texture;
                barrelAlbedoTexture->lazyLoad(true);
                material->setAlbedo(barrelAlbedoTexture);
            } else if (material->getNormal() == nullptr) {
                const auto barrelNormalTexture = mesh->getMesh()->getTextures()[1].texture;
                barrelNormalTexture->lazyLoad(true);
                material->setNormal(barrelNormalTexture);
                material->setNormalEnabled(true);
            }

            if (material->getAlbedo() != nullptr && material->getNormal() != nullptr) {
                initialized = true;
                setVisible(true);
            }
        }
    }
} // Model
