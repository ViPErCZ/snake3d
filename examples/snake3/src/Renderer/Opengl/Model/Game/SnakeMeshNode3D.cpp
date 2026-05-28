#include "SnakeMeshNode3D.h"

#include "Renderer/Opengl/Material/MaterialBuilder.h"
#include "Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "Renderer/Opengl/Material/Feature/ShadowFeature.h"
#include "Resource/MaterialLoader.h"

#include "Physic/SphereShape.h"
#include "Renderer/Opengl/Material/Uniform/TextureUniform.h"
#include "Renderer/Opengl/Material/Uniform/TimerUniform.h"
#include "Tools/Layers.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Renderer/Opengl/Model/Standard/SphereMesh.h"

using namespace CollisionShape;
using namespace Material;

namespace Model {
    namespace {
        void copyExplosionSourceMaterial(const shared_ptr<ShaderMaterial>& crashMaterial,
            const shared_ptr<BaseMaterial>& sourceMaterial) {
            const auto sourceInstance = dynamic_pointer_cast<Material::MaterialInstance>(sourceMaterial);
            if (!sourceInstance) {
                crashMaterial->setUniform("hasAlbedoTexture", false);
                crashMaterial->setUniform("hasFallbackColor", false);
                crashMaterial->setUniform("useMaterial", true);
                return;
            }

            std::shared_ptr<Manager::TextureManager> albedoTex;
            std::optional<glm::vec3> color;
            for (const auto& feature : sourceInstance->getFeatures()) {
                if (const auto albedoFeature = dynamic_pointer_cast<Feature::AlbedoFeature>(feature)) {
                    albedoTex = albedoFeature->getAlbedo();
                    color = albedoFeature->getColor();
                    break;
                }
            }
            const bool hasAlbedoTexture = albedoTex && albedoTex->hasTexture();
            crashMaterial->setUniform("hasAlbedoTexture", hasAlbedoTexture);
            crashMaterial->setUniform("useMaterial", !hasAlbedoTexture);
            crashMaterial->setUniform("hasFallbackColor", color.has_value());
            if (color.has_value()) {
                crashMaterial->setUniform("fallbackColor", *color);
            }
            if (hasAlbedoTexture) {
                crashMaterial->setAlbedo(albedoTex);
            }
        }
    }

    SnakeMeshNode3D::SnakeMeshNode3D(const shared_ptr<ContextState> &contextState, const shared_ptr<StandardMesh> &mesh,
                                     const shared_ptr<ResourceManager> &resourceManager, const shared_ptr<CollisionSystem3D> &collisionSystem)
        : MeshNode3D(contextState, mesh, resourceManager), collisionSystem(collisionSystem) {
        timerUniform = make_shared<TimerUniform>(true);
        timerUniform2 = make_shared<TimerUniform>(false);
        if (resourceManager) {
            const auto shadowsShader = resourceManager->getShader("shadowDepthShader");

            tileLightingFeature = make_shared<Feature::LightingFeature>(nullptr, std::vector<std::shared_ptr<PointLight>>{}, std::vector<std::shared_ptr<SpotLight>>{});
            // D3.5: JSON-driven spec; runtime-wired features (lighting/shadow/fog)
            // doplňujeme z živých objektů. Shader permutation hash je order-insensitive
            // (OR ShaderFeatureMask), takže insertion order není potřeba zachovávat.
            auto spec = resourceManager->loadMaterial("Assets/Materials/snake_tile.json");
            if (spec.hasLighting) {
                spec.builder.with(tileLightingFeature);
            }
            if (spec.hasShadow) {
                spec.builder.with(make_shared<Feature::ShadowFeature>(
                    resourceManager->getTexture("depth"), shadowsShader));
            }
            if (spec.hasFog) {
                spec.builder.with(resourceManager->getFogFeature());
            }
            tileMaterial = spec.builder.build(*resourceManager->getShaderRegistry());

            headMaterial = mesh->getMaterial();

            timer = std::make_unique<Timer>(false);
            const auto respawnShader = resourceManager->getShader("respawnShader");
            respawnMaterial = make_shared<ShaderMaterial>(respawnShader, shadowsShader);
            respawnMaterial->setShadow(resourceManager->getTexture("depth"));
            respawnMaterial->setUniform("u_useMaterial", false);
            respawnMaterial->setUniform("u_LightColor", glm::vec4(0.88, 0.05, 0.05, 1.0f));
            respawnMaterial->setUniform("u_Speed", 4.7f);
            respawnMaterial->setUniform("u_Delay", 0.1f);
            respawnMaterial->setUniform("u_FloatParameter", 0.1f);

            const auto explosionShader = resourceManager->getShader("explosion");
            crashMaterial = make_shared<ShaderMaterial>(explosionShader, shadowsShader);
            crashMaterial->setUniform("time", timerUniform2);
            crashMaterial->setUniform("fadeTime", 0.65f);
            crashMaterial->setUniform("explosionRadius", 0.35f);
            crashMaterial->setUniform("hasAlbedoTexture", false);
            crashMaterial->setUniform("hasFallbackColor", false);
            crashMaterial->setUniform("fallbackColor", glm::vec3(1.0f, 1.0f, 1.0f));
            crashMaterial->setShadow(resourceManager->getTexture("depth"));
            crashMaterial->setBlending(Blending::Translucent);

            headRespawnMaterial = make_shared<ShaderMaterial>(respawnShader, shadowsShader);
            headRespawnMaterial->setShadow(resourceManager->getTexture("depth"));
            headRespawnMaterial->setUniform("u_useMaterial", true);
            headRespawnMaterial->setUniform("u_Speed", 4.7f);
            headRespawnMaterial->setUniform("u_Delay", 0.1f);
            headRespawnMaterial->setUniform("u_FloatParameter", 0.1f);

            const auto textureUniform = make_shared<TextureUniform>(11, resourceManager->getTexture("fast_noise.bmp"));
            respawnMaterial->setUniform("u_NoiseTexture", textureUniform);
            respawnMaterial->setUniform("u_Time", timerUniform);
            respawnMaterial->setUniform("useBones", false);
            respawnMaterial->setUniform("useMaterial", true);

            headRespawnMaterial->setUniform("u_NoiseTexture", textureUniform);
            headRespawnMaterial->setUniform("u_Time", timerUniform);
            headRespawnMaterial->setUniform("useBones", false);
            headRespawnMaterial->setUniform("useMaterial", true);
        }
    }

    void SnakeMeshNode3D::respawn() {
        bodySegment = false;
        respawned = false;
        // Determine target collider visibility:
        //  - after a crash: restore the value crash() captured (= what the user
        //    had via CollisionShapeHandler before the explosion animation).
        //  - on every other respawn path (initial spawn, winner-screen reset,
        //    multiplayer rebuild): keep whatever the head shape currently shows
        //    so the user's inspector toggle is respected.
        const bool keepVisible = !collisionShapes.empty()
            ? (crashedSinceLastRespawn
                   ? collisionShapeVisibleBeforeCrash
                   : collisionShapes.begin()->get()->isVisible())
            : true;
        crashedSinceLastRespawn = false;
        if (!collisionShapes.empty()) {
            collisionShapes.begin()->get()->setVisible(keepVisible);
        }

        for (const auto &child: children) {
            collisionSystem->removeCollider(child);
        }

        children.clear();
        timerUniform->start();
        transformDetached = true;
        mesh->setMaterial(headRespawnMaterial);
        this->x = (23 - -23) / 2 * 32 + 16;
        this->y = (-3 - -23) / 2 * 32 + 16;
        this->setRotationX(90);
        this->setRotationY(0);
        this->setRotationZ(0);
        this->setPosition({23, -3, -23});
        this->setDirection(NONE);

        const auto tile = make_shared<SnakeMeshNode3D>(contextState, createTileNode(), resourceManager, collisionSystem);
        tile->setBodySegment(true);
        if (directionalLight) {
            tile->setDirectionalLight(directionalLight);
        }
        tile->setName("Snake tile 1");
        tile->setSpotLights(spotLights);
        tile->setPointLights(pointLights);
        tile->setPosition({21, -3, -23});
        tile->setScale({0.041667f, 0.041667f, 0.041667f});
        tile->x = x - 32;
        tile->y = y;

        const auto sphereShape = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape);
        shape->setName("Snake tile 1 - shape");
        shape->setCollisionLayer(PLAYER_BODY);
        shape->setCollisionMask(WORLD | ENEMY);
        tile->setCollisionShape(shape);
        collisionSystem->addCollider(tile);

        addNode(tile);

        const auto tile2 = make_shared<SnakeMeshNode3D>(contextState, createTileNode(), resourceManager, collisionSystem);
        tile2->setBodySegment(true);
        if (directionalLight) {
            tile2->setDirectionalLight(directionalLight);
        }
        tile2->setName("Snake tile 2");
        tile2->setSpotLights(spotLights);
        tile2->setPointLights(pointLights);
        tile2->setScale({0.041667f, 0.041667f, 0.041667f});
        tile2->setPosition({19, -3, -23});
        tile2->x = x - 64;
        tile2->y = y;

        const auto sphereShape2 = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape2 = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape2);
        shape2->setName("Snake tile 2 - shape");
        shape2->setCollisionLayer(PLAYER_BODY);
        shape2->setCollisionMask(WORLD | ENEMY);
        tile2->setCollisionShape(shape2);
        collisionSystem->addCollider(tile2);

        addNode(tile2);

        const auto tile3 = make_shared<SnakeMeshNode3D>(contextState, createTileNode(), resourceManager, collisionSystem);
        tile3->setBodySegment(true);
        if (directionalLight) {
            tile3->setDirectionalLight(directionalLight);
        }
        tile3->setName("Snake tile 3");
        tile3->setPointLights(pointLights);
        tile3->setSpotLights(spotLights);
        tile3->setScale({0.041667f, 0.041667f, 0.041667f});
        tile3->setPosition({17, -3, -23});
        tile3->x = x - 96;
        tile3->y = y;

        const auto sphereShape3 = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape3 = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape3);
        shape3->setName("Snake tile 3 - shape");
        shape3->setCollisionLayer(PLAYER_BODY);
        shape3->setCollisionMask(WORLD | ENEMY);
        tile3->setCollisionShape(shape3);
        collisionSystem->addCollider(tile3);

        addNode(tile3);

        const auto tile4 = make_shared<SnakeMeshNode3D>(contextState, createTileNode(), resourceManager, collisionSystem);
        tile4->setBodySegment(true);
        if (directionalLight) {
            tile4->setDirectionalLight(directionalLight);
        }
        tile4->setName("Snake tile 4");
        tile4->setPointLights(pointLights);
        tile4->setSpotLights(spotLights);
        tile4->setScale({0.041667f, 0.041667f, 0.041667f});
        tile4->setPosition({15, -3, -23});
        tile4->x = x - 128;
        tile4->y = y;

        const auto sphereShape4 = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape4 = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape4);
        shape4->setName("Snake tile 4 - shape");
        shape4->setCollisionLayer(PLAYER_BODY);
        shape4->setCollisionMask(WORLD | ENEMY | PLAYER);
        shape4->setRotationX(90.0f);
        tile4->setCollisionShape(shape4);
        collisionSystem->addCollider(tile4);

        addNode(tile4);

        const auto tile5 = make_shared<SnakeMeshNode3D>(contextState, createTileNode(), resourceManager, collisionSystem);
        tile5->setBodySegment(true);
        if (directionalLight) {
            tile5->setDirectionalLight(directionalLight);
        }
        tile5->setName("Snake tile 5");
        tile5->setPointLights(pointLights);
        tile5->setSpotLights(spotLights);
        tile5->setScale({0.041667f, 0.041667f, 0.041667f});
        tile5->setPosition({13, -3, -23});
        tile5->x = x - 160;
        tile5->y = y;

        const auto sphereShape5 = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape5 = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape5);
        shape5->setName("Snake tile 5 - shape");
        shape5->setCollisionLayer(PLAYER_BODY);
        shape5->setCollisionMask(WORLD | ENEMY | PLAYER);
        shape5->setRotationX(90.0f);
        tile5->setCollisionShape(shape5);
        collisionSystem->addCollider(tile5);

        addNode(tile5);

        // Propagate the resolved target visibility to every freshly-created
        // body tile so user toggles from CollisionShapeHandler are respected
        // even though the new shapes were never registered with the handler.
        for (const auto& child : children) {
            if (const auto t = dynamic_pointer_cast<SnakeMeshNode3D>(child)) {
                if (!t->getCollisionShapes().empty()) {
                    t->getCollisionShapes()[0]->setVisible(keepVisible);
                }
            }
        }
    }

    void SnakeMeshNode3D::setPostCrashRespawnHandler(std::function<void()> handler) {
        postCrashRespawnHandler = std::move(handler);
    }

    void SnakeMeshNode3D::crash() {
        if (timerUniform2->isRunning()) {
            return;
        }

        this->setDirection(NONE);
        copyExplosionSourceMaterial(crashMaterial, mesh->getMaterial());
        mesh->setMaterial(crashMaterial);
        collisionShapeVisibleBeforeCrash = collisionShapes.begin()->get()->isVisible();
        crashedSinceLastRespawn = true;
        collisionShapes.begin()->get()->setVisible(false);

        for (auto &child: children) {
            if (const auto tile = dynamic_pointer_cast<SnakeMeshNode3D>(child)) {
                tile->crash();
                tile->getCollisionShapes().at(0).get()->setVisible(false);
            }
        }

        timerUniform2->start();
    }

    void SnakeMeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
        if (resourceManager) {
            if (tileLightingFeature) tileLightingFeature->setDirectional(directional_light);
            respawnMaterial->setDirectionalLight(directionalLight);
            headRespawnMaterial->setDirectionalLight(directionalLight);
            crashMaterial->setDirectionalLight(directionalLight);
        }
        for (auto &child: children) {
            reinterpret_pointer_cast<SnakeMeshNode3D>(child)->setDirectionalLight(directional_light);
        }
    }

    void SnakeMeshNode3D::setSpotLights(const vector<shared_ptr<SpotLight>> &spot_light) {
        MeshNode3D::setSpotLights(spot_light);
        if (resourceManager) {
            if (tileLightingFeature) tileLightingFeature->setSpots(spotLights);
            respawnMaterial->setSpotLights(spotLights);
            headRespawnMaterial->setSpotLights(spotLights);
            crashMaterial->setSpotLights(spotLights);
        }
        for (auto &child: children) {
            reinterpret_pointer_cast<SnakeMeshNode3D>(child)->setSpotLights(spot_light);
        }
    }

    void SnakeMeshNode3D::setPointLights(const vector<shared_ptr<PointLight>> &point_light) {
        MeshNode3D::setPointLights(point_light);
        if (resourceManager) {
            if (tileLightingFeature) tileLightingFeature->setPoints(pointLights);
            respawnMaterial->setPointLights(point_light);
            headRespawnMaterial->setPointLights(point_light);
            crashMaterial->setPointLights(point_light);
        }
        for (auto &child: children) {
            reinterpret_pointer_cast<SnakeMeshNode3D>(child)->setPointLights(point_light);
        }
    }

    void SnakeMeshNode3D::setDirection(const eDIRECTION direction) {
        this->direction = direction;
    }

    void SnakeMeshNode3D::setBodySegment(const bool bodySegment) {
        this->bodySegment = bodySegment;
    }

    shared_ptr<SphereMesh> SnakeMeshNode3D::createTileNode() const {
        const auto sphere = make_shared<SphereMesh>(nullptr, 1.5, 0.75);
        const shared_ptr<BaseMaterial> material = timerUniform->isRunning() == false
            ? static_pointer_cast<BaseMaterial>(tileMaterial)
            : static_pointer_cast<BaseMaterial>(respawnMaterial);
        sphere->setMaterial(material);

        return sphere;
    }

    void SnakeMeshNode3D::addTile(const eDIRECTION direction) {
        glm::vec3 pos = {};
        const auto sphere = createTileNode();

        const auto PrevIter = children.end() - 1;

        if (getDirection() != STOP) {
            pos = (*PrevIter)->getPosition();
        } else {
            switch (direction) {
                case LEFT:
                    if ((*PrevIter)->getPosition().x - 2 >= -25) {
                        pos.x = (*PrevIter)->getPosition().x - 32;
                        pos.y = (*PrevIter)->getPosition().y;
                    }
                    break;
                case RIGHT:
                    if ((*PrevIter)->getPosition().x + 2 <= 752) {
                        pos.x = (*PrevIter)->getPosition().x + 32;
                        pos.y = (*PrevIter)->getPosition().y;
                    }
                    break;
                case UP:
                    if ((*PrevIter)->getPosition().y - 2 >= -25) {
                        pos.x = (*PrevIter)->getPosition().x;
                        pos.y = (*PrevIter)->getPosition().y - 32;
                    }
                    break;
                case DOWN:
                    if ((*PrevIter)->getPosition().y + 2 <= 752) {
                        pos.x = (*PrevIter)->getPosition().x;
                        pos.y = (*PrevIter)->getPosition().y + 32;
                    }
                    break;
                default:
                    break;
            }
        }

        const auto tile = make_shared<SnakeMeshNode3D>(contextState, sphere, resourceManager, collisionSystem);
        tile->setBodySegment(true);
        if (directionalLight) {
            tile->setDirectionalLight(directionalLight);
        }
        tile->setSpotLights(spotLights);
        tile->setPointLights(pointLights);
        tile->setPosition(pos);
        tile->setScale({0.041667f, 0.041667f, 0.041667f});
        tile->x = (*PrevIter)->x;
        tile->y = (*PrevIter)->y;

        const auto sphereShape = make_shared<SphereShape>(resourceManager, contextState,0.77f);
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, sphereShape);
        shape->setCollisionLayer(PLAYER_BODY);
        shape->setCollisionMask(WORLD | ENEMY | PLAYER);
        tile->setCollisionShape(shape);
        collisionSystem->addCollider(tile);

        // Inherit head's current collider visibility so newly-grown body
        // segments respect the user's CollisionShapeHandler toggle.
        if (!collisionShapes.empty()) {
            shape->setVisible(collisionShapes[0]->isVisible());
        }

        addNode(tile);
    }

    SnakeMeshNode3D::eDIRECTION SnakeMeshNode3D::getDirection() const {
        return direction;
    }

    void SnakeMeshNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
        const glm::mat4 &parentTransform, const bool shadows) {
        if (timerUniform2->isRunning() && timerUniform2->getElapsed() > 0.5f) {
            timerUniform2->stop();
            if (!bodySegment) {
                if (postCrashRespawnHandler) {
                    const auto handler = std::move(postCrashRespawnHandler);
                    postCrashRespawnHandler = nullptr;
                    handler();
                } else {
                    respawn();
                }
            } else {
                setVisible(false);
            }
        }

        // 0.8s matches the respawn shader's visible duration
        // (u_Delay + pi/u_Speed = 0.1 + pi/4.7 ~= 0.77s). The previous 0.5s
        // gate let input through while the spawn-in effect was still mid-way,
        // so the snake could be started before the animation finished.
        if (timerUniform->isRunning() && timerUniform->getElapsed() > 0.8f) {
            timerUniform->stop();
            mesh->setMaterial(headMaterial);
            for (auto &child: children) {
                if (const auto tile = dynamic_pointer_cast<SnakeMeshNode3D>(child)) {
                    tile->stopRespawn();
                }
            }
            respawned = true;
        }

        MeshNode3D::render(camera, projection, dt, parentTransform, shadows);

        if (!collisionShapes.empty()) {
            const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
            collisionShapes.begin()->get()->render(camera, projection, dt, finalTransform, shadows);
        }
    }

    void SnakeMeshNode3D::renderShadows(const shared_ptr<Camera> &camera, const glm::mat4 &projection, const float dt,
                                        const glm::mat4 &parentTransform) const {
        if (isCrashing()) {
            return;
        }
        MeshNode3D::renderShadows(camera, projection, dt, parentTransform);
    }

    void SnakeMeshNode3D::stopRespawn() {
        mesh->setMaterial(tileMaterial);
        respawned = true;
    }

    bool SnakeMeshNode3D::isReady() const {
        return mesh->getMaterial() == headMaterial && timerUniform->isRunning() == false;
    }

    bool SnakeMeshNode3D::isCrashing() const {
        return timerUniform2 && timerUniform2->isRunning();
    }

    bool SnakeMeshNode3D::isRespawning() const {
        return timerUniform && timerUniform->isRunning();
    }

    void SnakeMeshNode3D::setCollisionShape(const shared_ptr<CollisionShape3D> &collisionShape) {
        collisionShapes.clear();
        // Wire parent chain like addNode does -- inspector + physics lookup
        // need shape->getParent() to walk back to the owning head tile.
        collisionShape->setParent(shared_from_this());
        collisionShapes.push_back(collisionShape);
    }

    void SnakeMeshNode3D::computeWorldMatrix(const glm::mat4 &parentTransform) {
        MeshNode3D::computeWorldMatrix(parentTransform);

        for (const auto &shape : collisionShapes) {
            shape->computeWorldMatrix(parentTransform * this->getModelMatrix());
        }
    }

    void SnakeMeshNode3D::hide() {
        setVisible(false);

        for (const auto &child: children) {
            child->setVisible(false);
        }
    }
} // Model
