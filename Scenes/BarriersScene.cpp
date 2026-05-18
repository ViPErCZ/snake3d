#include "BarriersScene.h"

#include "../Physic/BoxShape.h"
#include "../Renderer/Opengl/Material/MaterialBuilder.h"
#include "../Renderer/Opengl/Material/Feature/AlbedoFeature.h"
#include "../Renderer/Opengl/Material/Feature/LightingFeature.h"
#include "../Renderer/Opengl/Material/Feature/NormalMapFeature.h"
#include "../Renderer/Opengl/Material/Feature/SpecularFeature.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"
#include "../Tools/Layers.h"

namespace Scenes {
    BarriersScene::BarriersScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void BarriersScene::init(const int priority) {
        Scene::init(priority);
        initBarriers();
        initLevelManager();
    }

    shared_ptr<LevelManager> BarriersScene::getLevelManager() const {
        return levelManager;
    }

    shared_ptr<MeshNode3D> BarriersScene::getLevelBoxes() const {
        return levelBoxes;
    }

    void BarriersScene::initBarriers() {
        const auto shader = resourceManager->getShader("basicShader");
        const auto boxMesh = make_shared<BoxMesh>(shader, 2.0, 2.0, 2.0);
        const auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
        const auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
        const auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");

        auto albedoFeature = make_shared<Feature::AlbedoFeature>(brickWall);
        albedoFeature->setAmbientIntensity(0.1f);
        const auto boxMaterial = Material::MaterialBuilder()
            .useMaster("basicShader")
            .with(make_shared<Feature::LightingFeature>(directionalLight, pointLights, spotLights))
            .with(make_shared<Feature::NormalMapFeature>(brickWallNormal))
            .with(make_shared<Feature::SpecularFeature>(brickWallSpecular))
            .with(albedoFeature)
            .build(*resourceManager->getShaderRegistry());

        boxMesh->setMaterial(boxMaterial);

        const auto boxNode3D = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
        boxNode3D->setPosition(glm::vec3{-25.0, -25.0, -23.0});
        boxNode3D->setScale({0.041666667f, 0.041666667f, 0.041666667f});
        boxNode3D->setName("PerimeterBottom 1");
        const auto boxShape = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(2.01, 2.01, 2.01));
        const auto shape = make_shared<CollisionShape3D>(contextState, resourceManager, boxShape);
        shape->setCollisionLayer(WORLD);
        shape->setCollisionMask(PLAYER | ENEMY);
        shape->setName("Perimeter shape 1");
        boxNode3D->addNode(shape);

        for (int x = 2; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{x, 0.0, 0.0});
            boxNode3D_2->setName("PerimeterBottom " + std::to_string(x));
            const auto boxShapeX = make_shared<BoxShape>(resourceManager, contextState,glm::vec3(2.01, 2.01, 2.01));
            const auto shapeX = make_shared<CollisionShape3D>(contextState, resourceManager, boxShapeX);
            shapeX->setCollisionLayer(WORLD);
            shapeX->setCollisionMask(PLAYER | ENEMY);
            shapeX->setName("Perimeter shape bottom " + std::to_string(x));
            boxNode3D_2->addNode(shapeX);
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int x = 0; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{x, 98.0, 0.0});
            boxNode3D_2->setName("PerimeterTop " + std::to_string(x));
            const auto boxShapeX = make_shared<BoxShape>(resourceManager, contextState, glm::vec3(2.01, 2.01, 2.01));
            const auto shapeX = make_shared<CollisionShape3D>(contextState, resourceManager, boxShapeX);
            shapeX->setCollisionLayer(WORLD);
            shapeX->setCollisionMask(PLAYER | ENEMY);
            shapeX->setName("Perimeter shape top " + std::to_string(x));
            boxNode3D_2->addNode(shapeX);
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{0, y, 0.0});
            boxNode3D_2->setName("PerimeterLeft " + std::to_string(y));
            const auto boxShapeY = make_shared<BoxShape>(resourceManager, contextState, glm::vec3(2.01, 2.01, 2.01));
            const auto shapeY = make_shared<CollisionShape3D>(contextState, resourceManager, boxShapeY);
            shapeY->setCollisionLayer(WORLD);
            shapeY->setCollisionMask(PLAYER | ENEMY);
            shapeY->setName("Perimeter shape left " + std::to_string(y));
            boxNode3D_2->addNode(shapeY);
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(contextState, boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{98, y, 0.0});
            boxNode3D_2->setName("PerimeterRight " + std::to_string(y));
            const auto boxShapeY = make_shared<BoxShape>(resourceManager, contextState, glm::vec3(2.01, 2.01, 2.01));
            const auto shapeY = make_shared<CollisionShape3D>(contextState, resourceManager, boxShapeY);
            shapeY->setCollisionLayer(WORLD);
            shapeY->setCollisionMask(PLAYER | ENEMY);
            shapeY->setName("Perimeter shape right " + std::to_string(y));
            boxNode3D_2->addNode(shapeY);
            boxNode3D->addNode(boxNode3D_2);
        }

        if (collisionSystem != nullptr) {
            collisionSystem->addCollider(boxNode3D);
        }
        perimeterBoxes = boxNode3D;

        addMeshNode3D(boxNode3D, 100);
    }

    void BarriersScene::initLevelManager() {
        levelManager = make_shared<LevelManager>(contextState, 1, MAX_LIVES, resourceManager);
        levelManager->setCollisionSystem(collisionSystem);
        levelBoxes = levelManager->createLevel(START_LEVEL, directionalLight, spotLights, pointLights);
        addMeshNode3D(levelBoxes, 3001);
    }

    void BarriersScene::nextLevel(const int targetLevel) {
        if (collisionSystem != nullptr) {
            if (levelBoxes) {
                collisionSystem->removeCollider(levelBoxes);
            }
            if (perimeterBoxes) {
                collisionSystem->removeCollider(perimeterBoxes);
            }
        }
        meshNode3d.clear();
        initBarriers();
        const int level = (targetLevel >= 0) ? targetLevel : levelManager->getLevel();
        levelBoxes = levelManager->createLevel(level, directionalLight, spotLights, pointLights);
        addMeshNode3D(levelBoxes, 3001);
    }
} // Scenes
