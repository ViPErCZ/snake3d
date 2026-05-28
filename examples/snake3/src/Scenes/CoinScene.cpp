#include "CoinScene.h"

#include "Physic/BoxShape.h"
#include "Renderer/Opengl/Material/MaterialBuilder.h"
#include "Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "Renderer/Opengl/Material/Feature/IblFeature.h"
#include "Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "Renderer/Opengl/Material/Feature/PbrFeature.h"
#include "Renderer/Opengl/Model/Standard/ArrayMesh.h"
#include <snake3d/Tools/Layers.h>

using namespace std;
using namespace Animation;
using namespace Physic;
using namespace Manager;
using namespace Lights;
using namespace Model;
using namespace Tools;
using namespace CollisionShape;

namespace Scenes {
    CoinScene::CoinScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void CoinScene::init(const int priority) {
        Scene::init(priority);
        initCoin();
    }

    shared_ptr<CoinMeshNode3D> CoinScene::getCoin() const {
        return coin;
    }

    shared_ptr<CoinMeshNode3D> CoinScene::getRemoveCoin() const {
        return removeCoin;
    }

    void CoinScene::update() {
        Scene::update();
    }

    void CoinScene::initCoin() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto coinMesh = make_shared<ArrayMesh>(shader);
        coinMesh->fromMesh(resourceManager->getModel("coin"));

        coin = make_shared<CoinMeshNode3D>(spotLights[4], contextState, coinMesh, resourceManager);
        coin->setPosition({-69.0, -69, -70.0f});
        coin->setScale({0.013888889, 0.013888889, 0.013888889});
        coin->setRotationX(90);
        coin->setVisible(false);
        coin->setName("coin");

        const auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
        const auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
        const auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
        const auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");

        auto coinLighting = make_shared<Feature::LightingFeature>(
            directionalLight,
            std::vector<std::shared_ptr<Lights::PointLight>>{},
            std::vector<std::shared_ptr<Lights::SpotLight>>{});

        auto coinAlbedoFeature = make_shared<Feature::AlbedoFeature>(coinAlbedo);
        coinAlbedoFeature->setAmbientIntensity(2.0f);

        const auto coinMaterial = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(coinLighting)
            .with(make_shared<Feature::NormalMapFeature>(coinNormal))
            .with(make_shared<Feature::PbrFeature>(coinMetalness, coinRoughness))
            .with(make_shared<Feature::IblFeature>(resourceManager->getTexture("skybox")))
            .with(coinAlbedoFeature)
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        coinMaterial->setBlending(Blending::Opaque);

        const auto boxShape = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(2.8, 2.8, 1.0));
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape);
        shape->setCollisionLayer(WORLD);
        shape->setCollisionMask(PLAYER | ENEMY);
        coin->addNode(shape);

        if (collisionSystem != nullptr) {
            collisionSystem->addCollider(coin);
        }

        if (manipulatorHandler != nullptr) {
            manipulatorHandler->getPositionHandler()->addItem(coin);
            manipulatorHandler->getScaleHandler()->addItem(coin);
            manipulatorHandler->getCollisionShapeHandler()->addItem(shape);
        }

        coinLighting->setSpots(spotLights);

        coinMesh->setMaterial(coinMaterial);

        // CREATE rotation animation
        std::vector<KeyFrame<glm::fquat>> rot_frames;
        rot_frames.emplace_back(glm::angleAxis(glm::radians(0.f),   glm::vec3(0,1,0)), 0.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(90.f),  glm::vec3(0,1,0)), 8.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(180.f), glm::vec3(0,1,0)), 16.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(270.f), glm::vec3(0,1,0)), 24.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(0.f),   glm::vec3(0,1,0)), 32.f);

        std::vector<KeyFrame<glm::vec3>> pos_frames;
        std::vector<KeyFrame<glm::vec3>> scale_frames;
        const auto animationNode = make_shared<AnimationNode>(pos_frames, rot_frames, scale_frames, nullptr);

        // CREATE eaten up animation
        std::vector<KeyFrame<glm::fquat>> rot_frames2;
        rot_frames2.emplace_back(glm::angleAxis(glm::radians(0.f),   glm::vec3(0,1,0)), 0.f);
        rot_frames2.emplace_back(glm::angleAxis(glm::radians(90.f),  glm::vec3(0,1,0)), 8.f);
        rot_frames2.emplace_back(glm::angleAxis(glm::radians(180.f), glm::vec3(0,1,0)), 16.f);

        std::vector<KeyFrame<glm::vec3>> pos_frames2;
        pos_frames2.emplace_back(glm::vec3(0.0f, 0.0, 0.0), 0);
        pos_frames2.emplace_back(glm::vec3(0.0f, 6.0, 0.0), 8);
        pos_frames2.emplace_back(glm::vec3(0.0f, 12.0, 0.0), 16);

        std::vector<KeyFrame<float>> alpha_frames;
        alpha_frames.emplace_back(1.0f, 0);
        alpha_frames.emplace_back(0.5f, 8);
        alpha_frames.emplace_back(0.0f, 16);

        const auto animationNode2 = make_shared<AnimationNode>(pos_frames2, rot_frames2, scale_frames, nullptr);
        animationNode2->setAlphaFrames(alpha_frames);

        const auto coinAnimation = make_shared<AnimationPlayer>();
        coinAnimation->createAnimation("coinRotation");
        coinAnimation->addAnimationNode("coinRotation", animationNode, 32);
        coinMesh->setAnimationPlayer(coinAnimation);

        const auto coinAnimation2 = make_shared<AnimationPlayer>();
        coinAnimation2->createAnimation("eatenUp");
        coinAnimation2->addAnimationNode("eatenUp", animationNode2, 16);
        const auto coinMesh2 = make_shared<ArrayMesh>(ArrayMesh(shader));
        coinMesh2->fromMesh(resourceManager->getModel("coin"));
        coinMesh2->setAnimationPlayer(coinAnimation2);
        coinMesh2->setMaterial(coinMaterial);
        removeCoin = make_shared<CoinMeshNode3D>(nullptr, contextState, coinMesh2, resourceManager);
        removeCoin->setPosition({-69.0, -69, -70.0f});
        removeCoin->setScale({0.013888889, 0.013888889, 0.013888889});
        removeCoin->setRotationX(90);
        removeCoin->setVisible(false);

        coinAnimation2->setCompletedCallback([this](AnimationPlayer * player) {
            removeCoin->setVisible(false);
            player->reset("eatenUp");
        });

        addMeshNode3D(coin);
        addMeshNode3D(removeCoin);
    }
} // Scenes
