#include "CoinScene.h"

#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"

namespace Scenes {
    CoinScene::CoinScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
    }

    void CoinScene::init() {
        Scene::init();
        initCoin();
    }

    shared_ptr<CoinMeshNode3D> CoinScene::getCoin() const {
        return coin;
    }

    void CoinScene::initCoin() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const std::shared_ptr<ObjItem> coinModel(
            resourceManager->getModel("coin"), [](ObjItem *) {
        });
        const auto coinMesh = make_shared<ArrayMesh>(ArrayMesh(shader));
        coinMesh->fromObj(coinModel);

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

        std::vector<KeyFrame<glm::vec3>> pos_frames;
        std::vector<KeyFrame<glm::fquat>> rot_frames;
        std::vector<KeyFrame<glm::vec3>> scale_frames;
        std::vector<KeyFrame<float>> alpha_frames;

        pos_frames.emplace_back(glm::vec3(0.0f, 0.0, 0.0), 0);
        pos_frames.emplace_back(glm::vec3(0.0f, 6.0, 0.0), 8);
        pos_frames.emplace_back(glm::vec3(0.0f, 12.0, 0.0), 16);
        pos_frames.emplace_back(glm::vec3(0.0f, 6.0, 0.0), 24);
        pos_frames.emplace_back(glm::vec3(0.0f, 0.0, 0.0), 32);

        // glm::angleAxis expects angle in radians
        rot_frames.emplace_back(glm::angleAxis(glm::radians(0.f),   glm::vec3(0,1,0)), 0.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(90.f),  glm::vec3(0,1,0)), 8.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(180.f), glm::vec3(0,1,0)), 16.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(270.f), glm::vec3(0,1,0)), 24.f);
        rot_frames.emplace_back(glm::angleAxis(glm::radians(0.f),   glm::vec3(0,1,0)), 32.f);

        alpha_frames.emplace_back(1.0f, 0);
        alpha_frames.emplace_back(0.5f, 8);
        alpha_frames.emplace_back(0.0f, 16);
        alpha_frames.emplace_back(0.5f, 24);
        alpha_frames.emplace_back(1.0f, 32);

        const auto animationNode = make_shared<AnimationNode>(pos_frames, rot_frames, scale_frames, nullptr);
        animationNode->setAlphaFrames(alpha_frames);
        coinAnimation = make_shared<AnimationPlayer>("coin");
        coinAnimation->addAnimationNode("coin", animationNode);
        coinMesh->setAnimationPlayer(coinAnimation);

        meshNode3d.push_back(coin);
    }
} // Scenes