#include "CoinScene.h"

#include "../Renderer/Opengl/Model/Standard/ArrayMesh.h"

namespace Scenes {
    CoinScene::CoinScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, int width, int height)
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

        coinMesh->setMaterial(coinMaterial);

        meshNode3d.push_back(coin);
    }
} // Scenes