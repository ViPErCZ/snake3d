#include "SnakeMeshNode3D.h"

#include "../../Material/Uniform/TextureUniform.h"
#include "../../Material/Uniform/TimerUniform.h"
#include "../Standard/AnimationArrayMesh.h"
#include "../Standard/SphereMesh.h"

namespace Model {
    SnakeMeshNode3D::SnakeMeshNode3D(const shared_ptr<StandardMesh> &mesh,
                                     const shared_ptr<ResourceManager> &resourceManager) : MeshNode3D(
        mesh, resourceManager) {
        timerUniform = make_shared<TimerUniform>(true);
        if (resourceManager) {
            const auto shader = resourceManager->getShader("basicShader");
            const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
            tileMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
            tileMaterial->setColor({0.88, 0.05, 0.05});
            tileMaterial->setShadow(resourceManager->getTexture("depth"));

            timer = std::make_unique<Timer>(false);
            const auto respawnShader = resourceManager->getShader("respawnShader");
            respawnMaterial = make_shared<ShaderMaterial>(respawnShader, shadowsShader);
            respawnMaterial->setShadow(resourceManager->getTexture("depth"));
            respawnMaterial->addUniform("u_LightColor", glm::vec4(0.88, 0.05, 0.05, 1.0f));
            respawnMaterial->addUniform("u_Speed", 4.7f);
            respawnMaterial->addUniform("u_Delay", 0.1f);
            respawnMaterial->addUniform("u_FloatParameter", 0.1f);

            const auto textureUniform = make_shared<TextureUniform>(11, this->resourceManager->getTexture("fast_noise.bmp"));
            respawnMaterial->addUniform("u_NoiseTexture", textureUniform);
            respawnMaterial->addUniform("u_Time", timerUniform);
            respawnMaterial->addUniform("useBones", false);
            respawnMaterial->addUniform("useMaterial", true);
        }
    }

    void SnakeMeshNode3D::respawn() {
        respawned = false;
        children.clear();
        timerUniform->start();
        transformDetached = true;
        this->x = (23 - -23) / 2 * 32 + 16;
        this->y = (-3 - -23) / 2 * 32 + 16;
        this->setRotationX(90);
        this->setRotationY(0);
        this->setPosition({23, -3, -23});
        this->setDirection(NONE);

        const auto sphere = createTileNode();

        const auto tile = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        if (directionalLight) {
            tile->setDirectionalLight(directionalLight);
        }
        tile->setPosition({21, -3, -23});
        tile->setScale({0.041667f, 0.041667f, 0.041667f});
        tile->x = x - 32;
        tile->y = y;
        addNode(tile);

        const auto tile2 = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        if (directionalLight) {
            tile2->setDirectionalLight(directionalLight);
        }
        tile2->setScale({0.041667f, 0.041667f, 0.041667f});
        tile2->setPosition({19, -3, -23});
        tile2->x = x - 64;
        tile2->y = y;
        addNode(tile2);

        const auto tile3 = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        if (directionalLight) {
            tile3->setDirectionalLight(directionalLight);
        }
        tile3->setScale({0.041667f, 0.041667f, 0.041667f});
        tile3->setPosition({17, -3, -23});
        tile3->x = x - 96;
        tile3->y = y;
        addNode(tile3);
    }

    void SnakeMeshNode3D::setDirectionalLight(const shared_ptr<DirectionalLight> &directional_light) {
        directionalLight = directional_light;
        tileMaterial->setDirectionalLight(directional_light);
        respawnMaterial->setDirectionalLight(directionalLight);
        for (auto &child: children) {
            reinterpret_pointer_cast<SnakeMeshNode3D>(child)->setDirectionalLight(directional_light);
        }
    }

    void SnakeMeshNode3D::setDirection(const eDIRECTION direction) {
        this->direction = direction;
    }

    void SnakeMeshNode3D::stop(const bool stop) const {
        try {
            dynamic_pointer_cast<AnimationArrayMesh>(mesh)->stop(stop);
        } catch (exception &e) {
            cout << "Mesh is not AnimationArrayMesh instance." << endl;
        }
    }

    shared_ptr<SphereMesh> SnakeMeshNode3D::createTileNode() const {
        const auto sphere = make_shared<SphereMesh>(nullptr, 1.5, 0.75);
        sphere->setMaterial(respawned ? tileMaterial : respawnMaterial);

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

        const auto tile = make_shared<SnakeMeshNode3D>(sphere, resourceManager);
        if (directionalLight) {
            tile->setDirectionalLight(directionalLight);
        }
        tile->setPosition(pos);
        tile->setScale({0.041667f, 0.041667f, 0.041667f});
        tile->x = (*PrevIter)->x;
        tile->y = (*PrevIter)->y;

        addNode(tile);
    }

    SnakeMeshNode3D::eDIRECTION SnakeMeshNode3D::getDirection() const {
        return direction;
    }

    void SnakeMeshNode3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform, const bool shadows) {
        if (timerUniform->getElapsed() > 0.5f) {
            timerUniform->stop();
            for (auto &child: children) {
                reinterpret_pointer_cast<SnakeMeshNode3D>(child)->stopRespawn();
            }
            respawned = true;
        }

        MeshNode3D::render(camera, projection, dt, parentTransform, shadows);
    }

    void SnakeMeshNode3D::stopRespawn() {
        mesh->setMaterial(tileMaterial);
        respawned = true;
    }

    bool SnakeMeshNode3D::isReady() const {
        return respawned;
    }
} // Model
