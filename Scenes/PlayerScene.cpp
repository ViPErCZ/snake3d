#include "PlayerScene.h"
#include "../Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"

namespace Scenes {

    PlayerScene::PlayerScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
            const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
            : Scene(rendererManager, camera, projection, rm, width, height) {
    }

    void PlayerScene::init() {
        Scene::init();
        initSnake();
        initSnakeMoveHandler();
    }

    shared_ptr<SnakeMeshNode3D> PlayerScene::getSnake() const {
        return snake;
    }

    shared_ptr<SnakeMoveHandler> PlayerScene::getSnakeMoveHandler() const {
        return snakeMoveHandler;
    }

    void PlayerScene::initSnake() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto pacmanMesh = make_shared<AnimationArrayMesh>(resourceManager->getAnimationModel("pacman"), shader, "KostraAction");

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 11.0f});
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.7f, 0.7f, 0.7f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto material = make_shared<StandardMaterial>(shader, shadowsShader);
        material->setShadow(resourceManager->getTexture("depth"));
        material->setNormalEnabled(true);
        material->setDirectionalLight(directionalLight);
        pacmanMesh->setMaterial(material);
        pacmanMesh->getAnimationPlayer()->setAcceleration(2.5f);

        snake = make_shared<SnakeMeshNode3D>(pacmanMesh, resourceManager);
        snake->setDirectionalLight(directionalLight);
        snake->setScale({0.041667f, 0.041667f, 0.041667f});
        snake->respawn();

        camera->setStickyPoint(snake);

        meshNode3d.push_back(snake);
    }

    void PlayerScene::initSnakeMoveHandler() {
        snakeMoveHandler = make_shared<SnakeMoveHandler>(snake);
        keyboardManager->addEventHandler(snakeMoveHandler);

        buildStartMoveCallback();
        buildStopMoveCallback();
    }

    void PlayerScene::buildStartMoveCallback() const {
        snakeMoveHandler->addStartMoveCallback([this]() {
            this->snake->animationStart("KostraAction");
        });
    }

    void PlayerScene::buildStopMoveCallback() const {
        snakeMoveHandler->setStopMoveCallback([this](const bool stop) {
            if (stop) {
                this->snake->animationPause("KostraAction");
            } else {
                this->snake->animationResume("KostraAction");
            }
        });
    }
} // Scenes