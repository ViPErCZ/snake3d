#include "LevelManager.h"
#include <fstream>

#include "../Physic/BoxShape.h"
#include "../Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "../Tools//Layers.h"

namespace Manager {
    LevelManager::LevelManager(const shared_ptr<ContextState> &contextState,
        const int level, const int live, const shared_ptr<ResourceManager> &resourceManager)
        : level(level), live(live), eatCounter(0), resourceManager(resourceManager), contextState(contextState) {
    }

    void LevelManager::setLevel(const int level) {
        LevelManager::level = level;
    }

    void LevelManager::setLive(const int live) {
        LevelManager::live = live;
    }

    int LevelManager::getLevel() const {
        return level;
    }

    int LevelManager::getLive() const {
        return live;
    }

    void LevelManager::resolveBoxShape(const shared_ptr<MeshNode3D> &boxNode3D) {
        const auto boxShape = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(2.01, 2.01, 2.01));
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape);
        shape->setCollisionLayer(WORLD);
        shape->setCollisionMask(PLAYER);
        shape->setName(boxNode3D->getName() + " - shape");
        boxNode3D->addNode(shape);
        if (collisionSystem) {
            collisionSystem->addCollider(boxNode3D);
        }
    }

    shared_ptr<MeshNode3D> LevelManager::createLevel(int level,
                                                     shared_ptr<DirectionalLight> &directionalLight,
                                                     const vector<shared_ptr<SpotLight> > &spotLights,
                                                     const vector<shared_ptr<PointLight> > &pointLights) {
        const auto shader = resourceManager ? resourceManager->getShader("basicShader") : nullptr;
        const auto shadowsShader = resourceManager ? resourceManager->getShader("shadowDepthShader") : nullptr;
        const auto boxMesh = make_shared<BoxMesh>(shader, 2.0, 2.0, 2.0);

        if (resourceManager) {
            const auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
            const auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
            const auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");
            const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
            boxMaterial->setColor({1.0, 1.0, 1.0});
            boxMaterial->setNormalEnabled(true);
            boxMaterial->setAlbedo(brickWall);
            boxMaterial->setNormal(brickWallNormal);
            boxMaterial->setSpecular(brickWallSpecular);
            boxMaterial->setDirectionalLight(directionalLight);
            boxMaterial->setSpotLights(spotLights);
            boxMaterial->setPointLights(pointLights);
            boxMesh->setMaterial(boxMaterial);
        }

        const auto boxNode3D = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
        boxNode3D->setPosition({0.0, 0.0, -23.0});
        boxNode3D->setScale({0.041666667f, 0.041666667f, 0.041666667f});
        boxNode3D->setTransformDetached(true);
        resolveBoxShape(boxNode3D);

        this->level = level;
        this->eatCounter = 0;

        string filename = "Assets/Levels/level";
        filename += std::to_string(level);
        filename += ".txt";

        ifstream infile(filename);
        if (infile.is_open()) {
            bool isFirst = true;
            std::string line;
            int y = 0;
            while (std::getline(infile, line)) {
                int x = 0;
                for (char &c: line) {
                    if (c == 49) {
                        // "1"
                        if (isFirst) {
                            boxNode3D->setPosition({-25 + ((x + 1) * 2), -25 + ((y + 1) * 2), -23.0});
                            boxNode3D->x = (x+1) * 32;
                            boxNode3D->y = (y+1) * 32;
                            isFirst = false;
                            x++;
                            continue;
                        }
                        const auto childBoxNode3D = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
                        childBoxNode3D->setPosition({-25 + ((x + 1) * 2), -25 + ((y + 1) * 2), -23.0});
                        childBoxNode3D->setScale({0.041666667f, 0.041666667f, 0.041666667f});
                        childBoxNode3D->x = (x+1) * 32;
                        childBoxNode3D->y = (y+1) * 32;
                        childBoxNode3D->setName("Level box " + std::to_string(x) + ", " + std::to_string(y));
                        resolveBoxShape(childBoxNode3D);
                        boxNode3D->addNode(childBoxNode3D);
                    }

                    x++;
                }

                y++;
            }
            infile.close();
        }

        return boxNode3D;
    }

    int LevelManager::getEatCounter() const {
        return eatCounter;
    }

    void LevelManager::setEatCounter(const int eatCounter) {
        LevelManager::eatCounter = eatCounter;
    }

    void LevelManager::setCollisionSystem(const shared_ptr<CollisionSystem3D> &collisionSystem) {
        this->collisionSystem = collisionSystem;
    }
} // Manager
