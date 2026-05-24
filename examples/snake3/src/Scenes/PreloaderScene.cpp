#include "PreloaderScene.h"

#include "Renderer/Opengl/Material/ShaderMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TimerUniform.h"
#include "Renderer/Opengl/Model/SpinnerMesh.h"
#include "Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

using Uniform::TimerUniform;

namespace Scenes {
    PreloaderScene::PreloaderScene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void PreloaderScene::init(const int priority) {
        Scene::init(priority);

        const auto preLoader = initPreloader();
        camera->setStickyPoint(preLoader);
        addMeshNode2D(preLoader);
    }

    shared_ptr<MeshNode2D> PreloaderScene::initPreloader() const {
        auto shader = resourceManager->getShader("preloadShader");
        const auto quad = make_shared<QuadNode2D>(width, height, shader);
        const auto material = make_shared<ShaderMaterial>(shader);
        const auto timerUnion = make_shared<TimerUniform>(true);
        material->setUniform("iTime", timerUnion);
        material->setUniform("iResolution", glm::vec2(width, height));
        quad->setMaterial(material);

        return make_shared<MeshNode2D>(contextState, quad, resourceManager);
    }
} // Scenes