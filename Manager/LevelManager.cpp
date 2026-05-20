#include "LevelManager.h"
#include <fstream>

#include "../Physic/BoxShape.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/ShadowFeature.h"
#include "../Renderer/Opengl/Material/Feature/SpecularFeature.h"
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
        shape->setCollisionMask(PLAYER | ENEMY);
        shape->setName(boxNode3D->getName() + " - shape");
        boxNode3D->addNode(shape);
        if (collisionSystem) {
            collisionSystem->addCollider(boxNode3D, true);
        }
    }

    void LevelManager::resolveFloorShape(const shared_ptr<MeshNode3D> &floorNode) {
        // Thin slab: 2x2 horizontal footprint matches the cell, 0.5 vertical so the
        // collider has just enough depth to be testable but does not push barriers up.
        const auto floorShape = make_shared<BoxShape>(resourceManager, contextState, glm::vec3(2.0, 2.0, 0.5));
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, floorShape);
        shape->setCollisionLayer(FLOOR);
        // Floor pairs with dynamic bodies that opted in by adding FLOOR to their
        // own mask (e.g. snake head). Floor-floor pairs are short-circuited via
        // the static-static skip in CollisionSystem3D::update().
        shape->setCollisionMask(PLAYER | ENEMY);
        shape->setName(floorNode->getName() + " - shape");
        floorNode->addNode(shape);
        if (collisionSystem) {
            collisionSystem->addCollider(floorNode, true);
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

            auto albedoFeature = make_shared<Feature::AlbedoFeature>(brickWall);
            albedoFeature->setColor(glm::vec3(1.0f));
            const auto boxMaterial = Material::MaterialBuilder()
                .useMaster("basicShader")
                .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, spotLights))
                .with(make_shared<Feature::ShadowFeature>(resourceManager->getTexture("depth"), shadowsShader))
                .with(make_shared<Feature::NormalMapFeature>(brickWallNormal))
                .with(make_shared<Feature::SpecularFeature>(brickWallSpecular))
                .with(albedoFeature)
                .with(resourceManager->getFogFeature())
                .build(*resourceManager->getShaderRegistry());
            boxMesh->setMaterial(boxMaterial);
        }

        const auto boxNode3D = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
        boxNode3D->setPosition({0.0, 0.0, -23.0});
        boxNode3D->setScale({0.041666667f, 0.041666667f, 0.041666667f});
        boxNode3D->setTransformDetached(true);
        resolveBoxShape(boxNode3D);

        this->level = level;
        this->eatCounter = 0;
        holes.clear();

        string filename = "Assets/Levels/level";
        filename += std::to_string(level);
        filename += ".txt";

        ifstream infile(filename);
        if (infile.is_open()) {
            bool isFirst = true;
            std::string line;
            int y = 0;
            constexpr float floorTopSetPosZ = -23.77f;
            constexpr float floorHalfExtentLocalZ = 0.25f;
            constexpr float floorCenterZ = floorTopSetPosZ - floorHalfExtentLocalZ;
            constexpr float barrierScale = 0.041666667f;
            while (std::getline(infile, line)) {
                int x = 0;
                for (char &c: line) {
                    // '2' = hole: no floor, no barrier - body falls through.
                    if (c == '2') {
                        holes.emplace_back(x, y);
                        x++;
                        continue;
                    }

                    // Every non-hole cell gets a floor collider (passable from above,
                    // catches falling bodies). Sits in its own FLOOR layer so the
                    // existing snake handler doesn't see it as a wall.
                    const float worldX = -25.0f + (static_cast<float>(x) + 1.0f) * 2.0f;
                    const float worldY = -25.0f + (static_cast<float>(y) + 1.0f) * 2.0f;
                    const auto floorNode = make_shared<MeshNode3D>(contextState, nullptr, resourceManager);
                    floorNode->setPosition({worldX, worldY, floorCenterZ});
                    floorNode->setScale({barrierScale, barrierScale, barrierScale});
                    floorNode->setName("Floor " + std::to_string(x) + "," + std::to_string(y));
                    floorNode->setVisible(false); // collider only - never rendered
                    resolveFloorShape(floorNode);
                    boxNode3D->addNode(floorNode);

                    if (c == '1') {
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

    bool LevelManager::isVoidAt(const int virtualX, const int virtualY) const {
        // Snake virtual coords use (gridX*32 + 16, gridY*32 + 16) for cell centers.
        // Anything outside the 48x48 grid is treated as void so the snake also
        // falls when it walks off the board edge.
        constexpr int gridSize = 48;
        const int gridX = (virtualX - 16) / 32;
        const int gridY = (virtualY - 16) / 32;
        if (gridX < 0 || gridX >= gridSize || gridY < 0 || gridY >= gridSize) {
            return true;
        }

        return std::ranges::any_of(holes, [gridX, gridY](const auto &cell) {
            return cell.x == gridX && cell.y == gridY;
        });
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
