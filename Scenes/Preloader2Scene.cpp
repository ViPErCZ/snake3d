#include "Preloader2Scene.h"

#include "../Renderer/Opengl/Model/SpinnerMesh.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

namespace Scenes {
    Preloader2Scene::Preloader2Scene(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight> > &spotLights,
        const vector<shared_ptr<PointLight> > &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera,
        const glm::mat4 &projection, const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void Preloader2Scene::init(const int priority) {
        Scene::init(priority);

        const auto preLoader = initPreloader();
        addMeshNode2D(preLoader);
    }

    shared_ptr<MeshNode2D> Preloader2Scene::initPreloader() const {
        auto shader = resourceManager->getShader("preloadShader2");
        const auto quad = make_shared<QuadNode2D>(width, height, shader);
        const auto material = make_shared<ShaderMaterial>(shader);
        const auto timerUniform = make_shared<TimerUniform>(true);
        material->setUniform("iTime", timerUniform);
        material->setUniform("iResolution", glm::vec2(width, height));
        quad->setMaterial(material);
        quad->setBlending(Blending::Translucent);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        return make_shared<MeshNode2D>(contextState, quad, resourceManager);
    }
} // Scenes
