#include "TorchScene.h"

#include <glm/gtc/random.hpp>

#include "Handler/Debug/PositionHandler.h"
#include "Physic/BoxShape.h"
#include "Physic/CylinderShape.h"
#include "Renderer/Opengl/Material/MaterialBuilder.h"
#include "Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "Renderer/Opengl/Material/Feature/ShadowFeature.h"
#include <snake3d/Resource/MaterialLoader.h>
#include "Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../Renderer/Opengl/Model/Game/BarrelNode3D.h"
#include "../Renderer/Opengl/Model/Game/StreetLampNode3D.h"
#include "Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/QuadMesh3D.h"

using namespace std;
using namespace Material;
using namespace Physic;
using namespace Manager;
using namespace Lights;
using namespace Model;
using namespace Tools;
using namespace CollisionShape;

namespace Scenes {
    TorchScene::TorchScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void TorchScene::init(const int priority) {
        Scene::init(priority);

        quad = make_shared<QuadMesh3D>(resourceManager->getShader("basicShader"), 1.7, 1.7);
        quad->setBlending(Blending::AlphaAdditive);
        quad->setDepthWrite(false);

        initTorch();

        const auto streetLamp = make_shared<StreetLampNode3D>(contextState, resourceManager);
        streetLamp->setDirectionalLight(directionalLight);
        streetLamp->setPointLights(pointLights);
        streetLamp->init();
        addMeshNode3D(streetLamp);

        const auto barrel = make_shared<BarrelNode3D>(contextState, resourceManager);
        barrel->setDirectionalLight(directionalLight);
        barrel->setPointLights(pointLights);
        barrel->init();
        barrel->setName("barrel");

        const auto shape = make_shared<CylinderShape>(resourceManager, contextState, 0.3f, 0.90f);
        const auto collisionShape = make_shared<CollisionShape3D>(contextState, resourceManager, shape);
        collisionShape->setName("Barrel shape");
        collisionShape->setPosition(glm::vec3(0, 0.40f, 0));

        barrel->addNode(collisionShape);

        if (collisionSystem != nullptr) {
            // Barrel je decor prop, nikdy se nepohybuje - static.
            collisionSystem->addCollider(barrel, true);
        }

        if (manipulatorHandler != nullptr) {
            manipulatorHandler->getPositionHandler()->addItem(streetLamp);
            manipulatorHandler->getPositionHandler()->addItem(barrel);
            manipulatorHandler->getCollisionShapeHandler()->addItem(collisionShape);
        }

        addMeshNode3D(barrel);
    }

    void TorchScene::initTorch() {
        const auto torch = make_shared<ArrayMesh>(resourceManager->getShader("basicShader"));
        torch->fromMesh(resourceManager->getModel("torch"));
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");

        // D3.7: JSON-driven spec. torch.png + torch_normal.png jsou v JSONu jako
        // static features (MaterialLoader si je sám resolvne přes ResourceManager);
        // runtime-wired lighting/shadow/fog doplňujeme z živých objektů.
        auto spec = resourceManager->loadMaterial("Assets/Materials/torch.json");
        if (spec.hasLighting) {
            spec.builder.with(make_shared<Feature::LightingFeature>(
                directionalLight,
                std::vector<std::shared_ptr<Lights::PointLight>>{},
                spotLights));
        }
        if (spec.hasShadow) {
            spec.builder.with(make_shared<Feature::ShadowFeature>(
                resourceManager->getTexture("depth"), shadowsShader));
        }
        if (spec.hasFog) {
            spec.builder.with(resourceManager->getFogFeature());
        }
        const auto torchMaterial = spec.builder.build(*resourceManager->getShaderRegistry());
        torchMaterial->setBlending(spec.blending);

        torch->setMaterial(torchMaterial);

        const auto torchNode = make_shared<MeshNode3D>(contextState, torch, resourceManager);
        const auto torchNode2 = make_shared<MeshNode3D>(contextState, torch, resourceManager);
        const auto torchNode3 = make_shared<MeshNode3D>(contextState, torch, resourceManager);
        const auto torchNode4 = make_shared<MeshNode3D>(contextState, torch, resourceManager);

        torchNode->setRotationX(90);
        torchNode2->setRotationX(90);
        torchNode3->setRotationX(90);
        torchNode4->setRotationX(90);
        torchNode->setScale({0.2, 0.2, 0.2});
        torchNode2->setScale({0.2, 0.2, 0.2});
        torchNode3->setScale({0.2, 0.2, 0.2});
        torchNode4->setScale({0.2, 0.2, 0.2});
        torchNode->setPosition({-5.07928, -5.47677, -3.48698});
        torchNode2->setPosition({15.2239, -5.47677, -3.48698});
        torchNode3->setPosition({15.2753, 15.4487, -3.48698});
        torchNode4->setPosition({-5.07928, 15.4487, -3.48698});

        const auto smoke = initSmoke();
        torchNode->addNode(smoke);
        torchNode2->addNode(smoke);
        torchNode3->addNode(smoke);
        torchNode4->addNode(smoke);

        const auto fire = initFire();
        torchNode->addNode(fire);
        torchNode2->addNode(fire);
        torchNode3->addNode(fire);
        torchNode4->addNode(fire);

        const auto boxShape = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(0.88, 1.9, 0.9));
        const auto boxShape2 = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(0.88, 1.9, 0.9));
        const auto boxShape3 = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(1, 2, 1));
        const auto boxShape4 = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(1, 2, 1));
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape);
        const auto shape2 = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape2);
        const auto shape3 = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape3);
        const auto shape4 = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape4);
        shape->setPosition(glm::vec3(0, -0.5f, 0));
        shape2->setPosition(glm::vec3(0, -0.5f, 0));
        shape3->setPosition(glm::vec3(0, -0.5f, 0));
        shape4->setPosition(glm::vec3(0, -0.5f, 0));
        //torchNode->addNode(shape);
        //torchNode2->addNode(shape2);
        //torchNode3->addNode(shape3);
        //torchNode4->addNode(shape4);
        // Torch decor props nikdy se nepohybují - static.
        collisionSystem->addCollider(torchNode, true);
        collisionSystem->addCollider(torchNode2, true);
        collisionSystem->addCollider(torchNode3, true);
        collisionSystem->addCollider(torchNode4, true);

        addMeshNode3D(torchNode, 1);
        addMeshNode3D(torchNode2, 1);
        addMeshNode3D(torchNode3, 1);
        addMeshNode3D(torchNode4, 1);

        if (manipulatorHandler != nullptr) {
            manipulatorHandler->getPositionHandler()->addItem(torchNode);
            manipulatorHandler->getPositionHandler()->addItem(torchNode2);
            manipulatorHandler->getPositionHandler()->addItem(torchNode3);
            manipulatorHandler->getPositionHandler()->addItem(torchNode4);

            manipulatorHandler->getScaleHandler()->addItem(torchNode);

            manipulatorHandler->getRotationHandler()->addItem(torchNode);
        }
    }

    shared_ptr<GPUParticle3D> TorchScene::initFire() {
        const auto gravity = glm::vec3(
            glm::linearRand(-0.005f, 0.005f),
            glm::linearRand(0.01f, 0.001f),
            glm::linearRand(0.005f, 0.009f)
        );

        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("fire.png");
        material->set_mode(Stretched);

        material->set_life_min(0.5f);
        material->set_life_max(1.0f);
        material->set_size_min(0.008f);
        material->set_size_max(0.042f);
        material->set_stretch(0.105f);
        material->set_vel_min({-0.005f, 0.000f, 0.100f});
        material->set_vel_max({ 0.005f, 0.010f, 0.200f});
        material->set_gravity(gravity);
        material->set_emitter_radius(0.03f);
        material->set_emitter_y_offset(0.24f);
        material->set_color_start({6.0f, 3.5f, 1.0f, 1.0f});
        material->set_color_end({7.0f, 4.5f, 1.5f, 0.0f});
        material->set_min_radius(0.05f);
        material->set_spawn_per_frame(0.6f);

        const auto fire = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 1000);
        fire->setPosition(glm::vec3(0.0, -0.1, 0.0));
        fire->setScale({2.2, 2.2, 2.2});
        fire->setRotationX(-90);

        return fire;
    }

    shared_ptr<GPUParticle3D> TorchScene::initSmoke() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("smoke.png");
        material->set_mode(Stretched);

        material->set_life_min(2.0f);
        material->set_life_max(0.5f);
        material->set_size_min(0.08f);
        material->set_size_max(0.042f);
        material->set_stretch(0.15f);
        material->set_vel_min({-0.005f, 0.000f, 0.100f});
        material->set_vel_max({ 0.005f, 0.010f, 0.200f});
        material->set_gravity(glm::vec3(0.0f, -0.05f, 0.0f));
        material->set_emitter_radius(0.03f);
        material->set_emitter_y_offset(0.24f);
        material->set_color_start({0.4f, 0.4f, 0.4f, 0.8f});
        material->set_color_end({0.2f, 0.2f, 0.2f, 0.0f});
        material->set_min_radius(0.05f);
        material->set_spawn_per_frame(0.6f);

        const auto smoke = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 10);
        smoke->setPosition(glm::vec3(0.0, 0.067, 0.0));
        smoke->setScale({2.0, 2.0, 2.0});
        smoke->setRotationX(-90);

        return smoke;
    }
} // Scenes