#include "PlayerScene.h"

#include "../Physic/BoxShape.h"
#include "../Physic/SphereShape.h"
#include "../Physic/Dynamics/DynamicBody.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/BonesFeature.h"
#include "../Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/ShadowFeature.h"
#include "../Tools/Layers.h"
#include "../Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"

namespace Scenes {
    PlayerScene::PlayerScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void PlayerScene::init(const int priority) {
        Scene::init(priority);
        initSnake();
        initSnakeMoveHandler();
    }

    shared_ptr<SnakeMeshNode3D> PlayerScene::getSnake() const {
        return snake;
    }

    shared_ptr<SnakeMoveHandler> PlayerScene::getSnakeMoveHandler() const {
        return snakeMoveHandler;
    }

    void PlayerScene::winning() const {
        snake->respawn();
        snake->hide();
    }

    void PlayerScene::setInputEnabled(const bool enabled) const {
        if (snakeMoveHandler) {
            snakeMoveHandler->setEnabled(enabled);
        }
    }

    void PlayerScene::initSnake() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto pacmanMesh = make_shared<AnimationArrayMesh>(resourceManager->getAnimationModel("pacman"), shader, "KostraAction");

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setDirection({1, 1.0, -3});
        directionalLight->setAmbient({0.2f, 0.2f, 0.2f});
        directionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        directionalLight->setSpecular({.091f, .091f, .091f});

        const auto material = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, spotLights))
            .with(make_shared<Feature::ShadowFeature>(resourceManager->getTexture("depth"), shadowsShader))
            .with(make_shared<Feature::NormalMapFeature>(nullptr))
            // Skeletal head needs FEATURE_BONES compiled in - basic.vs bones
            // branch is gated by it, and AnimationArrayMesh::renderMesh flips
            // useBones=true per skeletal sub-mesh. Without this the bones
            // transform path is stripped and the head renders half-broken
            // with no animation.
            .with(make_shared<Feature::BonesFeature>())
            .with(make_shared<Feature::AlbedoFeature>(nullptr))
            .with(resourceManager->getFogFeature())
            .build(*resourceManager->getShaderRegistry());
        pacmanMesh->setMaterial(material);
        pacmanMesh->getAnimationPlayer()->setAcceleration(2.5f);

        snake = make_shared<SnakeMeshNode3D>(contextState, pacmanMesh, resourceManager, collisionSystem);
        snake->setName("Snake head");
        snake->setDirectionalLight(directionalLight);
        snake->setScale({0.041667f, 0.041667f, 0.041667f});
        snake->setSpotLights(spotLights);
        snake->setPointLights(pointLights);
        snake->respawn();

        camera->setStickyPoint(snake);

        const auto sphereShape = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape);
        shape->setCollisionLayer(PLAYER);
        shape->setCollisionMask(WORLD | ENEMY | ENEMY_BODY | PLAYER_BODY | FLOOR);

        snake->setCollisionShape(shape);

        addMeshNode3D(snake);

        if (collisionSystem != nullptr) {
            collisionSystem->addCollider(snake);
            // The head is the only segment with a dynamic body - gravity catches
            // it the moment there's no floor underneath (level holes, board edge).
            snakeBody = make_shared<Physic::Dynamics::DynamicBody>();
            snakeBody->setUseGravity(true);
            // SnakeMoveHandler opts the body in once the snake is out of respawn
            // and (in multiplayer) under server control. Keeping it disabled here
            // prevents gravity from running before that handshake completes.
            snakeBody->setEnabled(false);
            collisionSystem->addDynamicBody(snake, snakeBody);
        }

        if (manipulatorHandler != nullptr) {
            manipulatorHandler->getCollisionShapeHandler()->addItem(shape);
        }
    }

    void PlayerScene::initSnakeMoveHandler() {
        snakeMoveHandler = make_shared<SnakeMoveHandler>(snake);
        snakeMoveHandler->setDynamicBody(snakeBody);
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
