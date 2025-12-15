#include "PreloaderScene.h"

#include "../Renderer/Opengl/Model/SpinnerMesh.h"

namespace Scenes {
    PreloaderScene::PreloaderScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
    }

    void PreloaderScene::init() {
        Scene::init();

        const auto preLoader = initPreloader();
        camera->setStickyPoint(preLoader);
        meshNode3d.push_back(preLoader);
    }

    shared_ptr<MeshNode3D> PreloaderScene::initPreloader() const {
        auto shader = resourceManager->getShader("preloadShader");
        auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");

        return make_shared<MeshNode3D>(make_shared<SpinnerMesh>(shader), resourceManager);
    }
} // Scenes