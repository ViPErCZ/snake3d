#include "OrbitSceneBase.h"

namespace Scenes {
    namespace {
        constexpr glm::vec3 kDefaultOrbitCenter = {1.0f, 1.0f, -1.0f};
        constexpr float kDefaultOrbitSpeed = 0.012f;
        constexpr float kDefaultOrbitBackOffset = 3.0f;
        constexpr glm::vec3 kDefaultOrbitWorldUp = {0.0f, 0.0f, 1.0f};
    }

    OrbitSceneBase::OrbitSceneBase(
        const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight>> &spotLights,
        const vector<shared_ptr<PointLight>> &pointLights,
        const shared_ptr<RenderManager> &rendererManager,
        const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void OrbitSceneBase::init(const int priority) {
        Scene::init(priority);
        configureOrbit();
    }

    void OrbitSceneBase::update() {
        Scene::update();
        cameraOrbit.update(camera, deltaTime);
    }

    void OrbitSceneBase::configureOrbit() {
        cameraOrbit.setCenter(getOrbitCenter());
        cameraOrbit.setBackOffset(getOrbitBackOffset());
        cameraOrbit.setSpeed(getOrbitSpeed());
        cameraOrbit.setWorldUp(getOrbitWorldUp());
    }

    glm::vec3 OrbitSceneBase::getOrbitCenter() const {
        return kDefaultOrbitCenter;
    }

    float OrbitSceneBase::getOrbitSpeed() const {
        return kDefaultOrbitSpeed;
    }

    float OrbitSceneBase::getOrbitBackOffset() const {
        return kDefaultOrbitBackOffset;
    }

    glm::vec3 OrbitSceneBase::getOrbitWorldUp() const {
        return kDefaultOrbitWorldUp;
    }
} // Scenes
