#include "PlayerScene.h"

#include "../Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "../Renderer/Opengl/Model/Standard/SphereMesh.h"

namespace Scenes {

    PlayerScene::PlayerScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
            : Scene(rendererManager, camera, projection, rm, width, height) {
    }

    void PlayerScene::init() {
        Scene::init();
        initSnake();
    }

    void PlayerScene::initSnake() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        auto geometry = make_shared<BaseItem>();
        geometry->setZoom({0.041667f, 0.041667f, 0.041667f});
        geometry->setPosition( {23, -3, -23});
        const auto sphere = make_shared<SphereMesh>(geometry, shader, 1.5, 0.75);

        const auto pacmanMesh = make_shared<AnimationArrayMesh>(AnimationArrayMesh(resourceManager->getAnimationModel("pacman"), shader));
        // pacmanMesh->getBaseItem()->setZoom({0.041667f, 0.041667f, 0.041667f});
        // pacmanMesh->getBaseItem()->setPosition( {23, -3, -23});
        pacmanMesh->getBaseItem()->setRotate(glm::vec4(1, 0, 0, 90), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0));

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto material = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        //material->setColor({0.88, 0.05, 0.05});
        material->setShadow(resourceManager->getTexture("depth"));
        material->setNormalEnabled(true);
        material->setDirectionalLight(directionalLight);
        pacmanMesh->setMaterial(material);

        snake = make_shared<SnakeMeshNode3D>(pacmanMesh, resourceManager);
        snake->setDirectionalLight(directionalLight);
        snake->respawn();

        meshes.push_back(snake);
    }
} // Scenes