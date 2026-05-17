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

    std::vector<glm::vec3> RemoteSnakeScene::collectPositions() const {
        std::vector<glm::vec3> positions;
        if (!snake) {
            return positions;
        }

        positions.push_back(snake->getPosition());
        for (const auto &child : snake->getChildren()) {
            positions.push_back(child->getPosition());
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

        snake->setPostCrashRespawnHandler([this, headPosition, direction]() {
            snake->respawn();
            if (snakeMoveHandler) {
                snakeMoveHandler->resetState();
            }
            setSpawnLayout(headPosition, direction);
            snake->animationStop("KostraAction");
        });
        snake->crash();
    }

    int RemoteSnakeScene::toVirtualCoord(const float worldCoord) {
        return static_cast<int>(std::lround((worldCoord + 23.0f) * 16.0f)) + 16;
    }

    void RemoteSnakeScene::initSnake() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto pacmanMesh = make_shared<AnimationArrayMesh>(resourceManager->getAnimationModel("pacman")->clone(), shader, "KostraAction");

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
        // Add FLOOR to the mask so this snake also sits on the floor cells and
        // falls through holes - same physics behaviour as the local snake.
        shape->setCollisionMask(WORLD | PLAYER | PLAYER_BODY | ENEMY_BODY | FLOOR);
        snake->setCollisionShape(shape);

        addMeshNode3D(snake);

        if (collisionSystem != nullptr) {
            collisionSystem->addCollider(snake);
            snakeBody = make_shared<Physic::Dynamics::DynamicBody>();
            snakeBody->setUseGravity(true);
            // Stay disabled until SnakeMoveHandler opts the body in (= snake is
            // server-controlled and out of respawn). Avoids gravity running
            // in the lobby / menu phase before startNetworkGame swaps the
            // handler on, which used to slingshot the head off into the void.
            snakeBody->setEnabled(false);
            collisionSystem->addDynamicBody(snake, snakeBody);
        }
        configureCollisionLayers();
    }

    void RemoteSnakeScene::initMoveHandler() {
        snakeMoveHandler = make_shared<SnakeMoveHandler>(snake);
        snakeMoveHandler->setDynamicBody(snakeBody);
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

        // FLOOR must stay in both masks so gravity-driven snap-to-floor works
        // (this function is called from init / ensureLength / respawnAt and
        // would otherwise overwrite the FLOOR bit set in initSnake).
        for (const auto &shapeNode : snake->getCollisionShapes()) {
            shapeNode->setCollisionLayer(ENEMY);
            shapeNode->setCollisionMask(WORLD | PLAYER | PLAYER_BODY | ENEMY_BODY | FLOOR);
        }

        for (const auto &child : snake->getChildren()) {
            for (const auto &shapeNode : child->getCollisionShapes()) {
                shapeNode->setCollisionLayer(ENEMY_BODY);
                shapeNode->setCollisionMask(WORLD | PLAYER | ENEMY | FLOOR);
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
