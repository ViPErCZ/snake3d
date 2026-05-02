#include "RemoteSnakeScene.h"

#include <cmath>

#include "../Physic/SphereShape.h"
#include "../Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "../Tools/Layers.h"

namespace Scenes {
    RemoteSnakeScene::RemoteSnakeScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void RemoteSnakeScene::init(const int priority) {
        Scene::init(priority);
        initSnake();
        initMoveHandler();
        setActive(false);
        setServerControlled(false);
    }

    shared_ptr<SnakeMeshNode3D> RemoteSnakeScene::getSnake() const {
        return snake;
    }

    shared_ptr<SnakeMoveHandler> RemoteSnakeScene::getMoveHandler() const {
        return snakeMoveHandler;
    }

    void RemoteSnakeScene::setServerControlled(const bool enabled) const {
        if (snakeMoveHandler) {
            snakeMoveHandler->setEnabled(enabled);
        }
    }

    void RemoteSnakeScene::setActive(const bool active) const {
        if (!snake) {
            return;
        }
        setVisibleRecursive(snake, active);
        setCollisionEnabledRecursive(snake, active);
    }

    void RemoteSnakeScene::setSpawnLayout(const glm::vec3 &headPosition, const SnakeMeshNode3D::eDIRECTION direction) const {
        if (!snake) {
            return;
        }

        ensureLength(6);
        if (snakeMoveHandler) {
            const auto bodyDirection = direction == SnakeMeshNode3D::NONE ? SnakeMeshNode3D::RIGHT : direction;
            snakeMoveHandler->setInitialBodyDirection(bodyDirection);
        }
        snake->setDirection(SnakeMeshNode3D::NONE);
        applyDirectionVisual(snake, direction == SnakeMeshNode3D::NONE ? SnakeMeshNode3D::RIGHT : direction);
        snake->setPosition(headPosition);
        snake->x = toVirtualCoord(headPosition.x);
        snake->y = toVirtualCoord(headPosition.y);

        glm::vec2 tailDelta{-2.0f, 0.0f};
        switch (direction) {
            case SnakeMeshNode3D::LEFT:
                tailDelta = {2.0f, 0.0f};
                break;
            case SnakeMeshNode3D::RIGHT:
            case SnakeMeshNode3D::NONE:
                tailDelta = {-2.0f, 0.0f};
                break;
            case SnakeMeshNode3D::UP:
                tailDelta = {0.0f, -2.0f};
                break;
            case SnakeMeshNode3D::DOWN:
                tailDelta = {0.0f, 2.0f};
                break;
            default:
                break;
        }

        size_t index = 1;
        for (const auto &child : snake->getChildren()) {
            const auto tile = dynamic_pointer_cast<SnakeMeshNode3D>(child);
            if (!tile) {
                continue;
            }

            const glm::vec3 pos = {
                headPosition.x + tailDelta.x * static_cast<float>(index),
                headPosition.y + tailDelta.y * static_cast<float>(index),
                headPosition.z
            };
            tile->setPosition(pos);
            tile->x = toVirtualCoord(pos.x);
            tile->y = toVirtualCoord(pos.y);
            tile->setDirection(SnakeMeshNode3D::NONE);
            ++index;
        }
        configureCollisionLayers();
    }

    std::vector<glm::vec2> RemoteSnakeScene::collectPositions() const {
        std::vector<glm::vec2> positions;
        if (!snake) {
            return positions;
        }

        positions.emplace_back(snake->getPosition().x, snake->getPosition().y);
        for (const auto &child : snake->getChildren()) {
            positions.emplace_back(child->getPosition().x, child->getPosition().y);
        }
        return positions;
    }

    void RemoteSnakeScene::applyNetworkInput(const int moveX, const int moveY, const uint8_t actions) const {
        if (!snakeMoveHandler) {
            return;
        }

        int key = 0;
        if (moveX < 0) {
            key = GLFW_KEY_J;
        } else if (moveX > 0) {
            key = GLFW_KEY_L;
        } else if (moveY > 0) {
            key = GLFW_KEY_I;
        } else if (moveY < 0) {
            key = GLFW_KEY_K;
        }

        if (key != 0) {
            snakeMoveHandler->onEventHandler(key, 0, GLFW_PRESS, 0, 0.0f);
        }
        if ((actions & 0x1u) != 0u) {
            snakeMoveHandler->onEventHandler(GLFW_KEY_SPACE, 0, GLFW_PRESS, 0, 0.0f);
        }
    }

    void RemoteSnakeScene::updateAuthoritative() const {
        if (snakeMoveHandler) {
            snakeMoveHandler->onDefaultHandler();
        }
    }

    void RemoteSnakeScene::respawnAt(const glm::vec3 &headPosition, const SnakeMeshNode3D::eDIRECTION direction) const {
        if (!snake) {
            return;
        }

        snake->respawn();
        if (snakeMoveHandler) {
            snakeMoveHandler->resetState();
        }
        setSpawnLayout(headPosition, direction);
        snake->animationStop("KostraAction");
    }

    int RemoteSnakeScene::toVirtualCoord(const float worldCoord) {
        return static_cast<int>(std::lround((worldCoord + 23.0f) * 16.0f)) + 16;
    }

    void RemoteSnakeScene::initSnake() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto pacmanMesh = make_shared<AnimationArrayMesh>(resourceManager->getAnimationModel("pacman"), shader, "KostraAction");

        const auto localDirectionalLight = make_shared<DirectionalLight>();
        localDirectionalLight->setDirection({1, 1.0, -3});
        localDirectionalLight->setAmbient({0.2f, 0.2f, 0.2f});
        localDirectionalLight->setDiffuse({0.1f, 0.1f, 0.1f});
        localDirectionalLight->setSpecular({.091f, .091f, .091f});

        const auto material = make_shared<StandardMaterial>(shader, shadowsShader);
        material->setShadow(resourceManager->getTexture("depth"));
        material->setNormalEnabled(true);
        material->setDirectionalLight(localDirectionalLight);
        material->setSpotLights(spotLights);
        material->setPointLights(pointLights);
        pacmanMesh->setMaterial(material);
        pacmanMesh->getAnimationPlayer()->setAcceleration(2.5f);

        snake = make_shared<SnakeMeshNode3D>(contextState, pacmanMesh, resourceManager, collisionSystem);
        snake->setName("Remote snake");
        snake->setDirectionalLight(localDirectionalLight);
        snake->setScale({0.041667f, 0.041667f, 0.041667f});
        snake->setSpotLights(spotLights);
        snake->setPointLights(pointLights);
        snake->respawn();

        const auto sphereShape = make_shared<SphereShape>(resourceManager, contextState, 0.77f);
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape);
        shape->setCollisionLayer(ENEMY);
        shape->setCollisionMask(WORLD | PLAYER | PLAYER_BODY | ENEMY_BODY);
        snake->setCollisionShape(shape);

        addMeshNode3D(snake);

        if (collisionSystem != nullptr) {
            collisionSystem->addCollider(snake);
        }
        configureCollisionLayers();
    }

    void RemoteSnakeScene::initMoveHandler() {
        snakeMoveHandler = make_shared<SnakeMoveHandler>(snake);
        snakeMoveHandler->addStartMoveCallback([this]() {
            if (snake) {
                snake->animationStart("KostraAction");
            }
        });
        snakeMoveHandler->setStopMoveCallback([this](const bool stop) {
            if (!snake) {
                return;
            }
            if (stop) {
                snake->animationPause("KostraAction");
            } else {
                snake->animationResume("KostraAction");
            }
        });
    }

    void RemoteSnakeScene::ensureLength(const size_t segmentCount) const {
        if (!snake || segmentCount == 0) {
            return;
        }

        const size_t currentCount = snake->getChildren().size() + 1;
        for (size_t i = currentCount; i < segmentCount; ++i) {
            snake->addTile(SnakeMeshNode3D::RIGHT);
        }
        configureCollisionLayers();
    }

    void RemoteSnakeScene::configureCollisionLayers() const {
        if (!snake) {
            return;
        }

        for (const auto &shapeNode : snake->getCollisionShapes()) {
            shapeNode->setCollisionLayer(ENEMY);
            shapeNode->setCollisionMask(WORLD | PLAYER | PLAYER_BODY | ENEMY_BODY);
        }

        for (const auto &child : snake->getChildren()) {
            for (const auto &shapeNode : child->getCollisionShapes()) {
                shapeNode->setCollisionLayer(ENEMY_BODY);
                shapeNode->setCollisionMask(WORLD | PLAYER | ENEMY);
            }
        }
    }

    void RemoteSnakeScene::applyDirectionVisual(const shared_ptr<SnakeMeshNode3D> &node, const SnakeMeshNode3D::eDIRECTION direction) {
        if (!node) {
            return;
        }

        node->setRotationX(90.0f);
        switch (direction) {
            case SnakeMeshNode3D::LEFT:
                node->setRotationY(180.0f);
                break;
            case SnakeMeshNode3D::RIGHT:
            case SnakeMeshNode3D::NONE:
                node->setRotationY(0.0f);
                break;
            case SnakeMeshNode3D::UP:
                node->setRotationY(90.0f);
                break;
            case SnakeMeshNode3D::DOWN:
                node->setRotationY(-90.0f);
                break;
            default:
                break;
        }
    }

    void RemoteSnakeScene::setVisibleRecursive(const shared_ptr<MeshNode3D> &node, const bool visible) {
        if (!node) {
            return;
        }

        node->setVisible(visible);
        for (const auto &child : node->getChildren()) {
            setVisibleRecursive(child, visible);
        }
    }

    void RemoteSnakeScene::setCollisionEnabledRecursive(const shared_ptr<MeshNode3D> &node, const bool enabled) {
        if (!node) {
            return;
        }

        for (const auto &shapeNode : node->getCollisionShapes()) {
            if (const auto shape = shapeNode->getShape()) {
                shape->setCollisionEnabled(enabled);
            }
        }

        for (const auto &child : node->getChildren()) {
            setCollisionEnabledRecursive(child, enabled);
        }
    }
} // Scenes
