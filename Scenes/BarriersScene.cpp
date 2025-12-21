#include "BarriersScene.h"
#include "../Renderer/Opengl/Model/Standard/BoxMesh.h"

namespace Scenes {
    BarriersScene::BarriersScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
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
        const auto shadowsShader = resourceManager->getShader("shadowDepthShader");
        const auto boxMesh = make_shared<BoxMesh>(shader, 2.0, 2.0, 2.0);

        const auto directionalLight = make_shared<DirectionalLight>();
        directionalLight->setPosition({0.0f, 7.0f, 110.0f});
        directionalLight->setDirection({0, 1.0, -3});
        directionalLight->setAmbient({0.1f, 0.1f, 0.1f});
        directionalLight->setDiffuse({0.005f, 0.005f, 0.005f});
        directionalLight->setSpecular({.01f, .01f, .01f});

        const auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
        const auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
        const auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");
        const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(shader, shadowsShader));
        boxMaterial->setNormalEnabled(true);
        boxMaterial->setAlbedo(brickWall);
        boxMaterial->setNormal(brickWallNormal);
        boxMaterial->setSpecular(brickWallSpecular);
        boxMaterial->setColor({1.0, 1.0, 1.0});
        boxMaterial->setAmbientLightColorIntensity(0.1);
        boxMaterial->setDirectionalLight(directionalLight);

        boxMesh->setMaterial(boxMaterial);

        const auto boxNode3D = make_shared<MeshNode3D>(boxMesh, resourceManager);
        boxNode3D->setPosition(glm::vec3{-25.0, -25.0, -23.0});
        boxNode3D->setScale({0.041666667f, 0.041666667f, 0.041666667f});

        for (int x = 2; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{x, 0.0, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int x = 0; x <= 98; x += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{x, 98.0, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{0, y, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        for (int y = 2; y <= 96; y += 2) {
            const auto boxNode3D_2 = make_shared<MeshNode3D>(boxMesh, resourceManager);
            boxNode3D_2->setPosition(glm::vec3{98, y, 0.0});
            boxNode3D->addNode(boxNode3D_2);
        }

        addMeshNode3D(boxNode3D, 100);
    }

    void BarriersScene::initLevelManager() {
        levelManager = make_shared<LevelManager>(1, MAX_LIVES, resourceManager);
        levelManager->createLevel(START_LEVEL);
        levelBoxes = levelManager->createLevel(START_LEVEL);
        addMeshNode3D(levelBoxes, 3001);
    }
} // Scenes