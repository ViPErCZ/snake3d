#include "CoinScene.h"

#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"

namespace Scenes {
    CoinScene::CoinScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
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

    void CoinScene::initCoin() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto coinMesh = make_shared<ArrayMesh>(shader);
        coinMesh->fromMesh(resourceManager->getModel("coin"));

        coin = make_shared<CoinMeshNode3D>(coinMesh, resourceManager);
        coin->setPosition({-69.0, -69, -70.0f});
        coin->setScale({0.013888889, 0.013888889, 0.013888889});
        coin->setRotationX(90);
        coin->setVisible(false);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
        const auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
        const auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
        auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");
        const auto coinMaterial = make_shared<StandardMaterial>(shader, shadowsShader);
        coinMaterial->setAlbedo(coinAlbedo);
        coinMaterial->setNormal(coinNormal);
        coinMaterial->setSpecular(coinMetalness);
        coinMaterial->setNormalEnabled(true);
        coinMaterial->setDirectionalLight(directionalLight);
        coinMaterial->setBlending(Blending::Translucent);

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

        // CREATE eaten up animationi
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
        removeCoin = make_shared<CoinMeshNode3D>(coinMesh2, resourceManager);
        removeCoin->setPosition({-69.0, -69, -70.0f});
        removeCoin->setScale({0.013888889, 0.013888889, 0.013888889});
        removeCoin->setRotationX(90);
        removeCoin->setVisible(false);

        coinAnimation2->setCompletedCallback([this](AnimationPlayer * player) {
            removeCoin->setVisible(false);
            player->reset("eatenUp");
        });

        // meshNode3d.push_back(coin);
        // meshNode3d.push_back(removeCoin);
        addMeshNode3D(coin);
        addMeshNode3D(removeCoin);
    }
} // Scenes