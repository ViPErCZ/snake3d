#include "WinnerScene.h"

#include <algorithm>
#include <glm/gtc/random.hpp>

#include "Renderer/Opengl/Material/Particle/ParticleProcessMaterial.h"
#include "Renderer/Opengl/Model/Standard/GPUParticle3D.h"

using namespace Material;

namespace Scenes {
    namespace {
        constexpr int kExplosionCount = 7;
        constexpr float kTimeOffsetStep = 0.2f;
        constexpr float kTimeScale = 0.42f;
        constexpr float kResetLead = 0.05f;

        constexpr float kPlaneMinX = -1.0f;
        constexpr float kPlaneMaxX = 3.0f;
        constexpr float kPlaneMinY = -1.0f;
        constexpr float kPlaneMaxY = 3.0f;
        constexpr float kExplosionZ = 0.0f;

    }

    WinnerScene::WinnerScene(const shared_ptr<DirectionalLight> &directionalLight,
        const vector<shared_ptr<SpotLight>> &spotLights, const vector<shared_ptr<PointLight>> &pointLights,
        const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : OrbitSceneBase(directionalLight, spotLights, pointLights, rendererManager, camera, projection, rm, width, height) {
    }

    void WinnerScene::init(const int priority) {
        OrbitSceneBase::init(priority);

        quad = make_shared<QuadMesh3D>(resourceManager->getShader("basicShader"), 1.7, 1.7);
        quad->setBlending(Blending::AlphaAdditive);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        initExplosion();
    }

    void WinnerScene::update() {
        OrbitSceneBase::update();

        if (explosions.empty()) {
            return;
        }

        for (size_t i = 0; i < explosions.size(); ++i) {
            explosionMoveTimers[i] -= deltaTime / 70;
            if (explosionMoveTimers[i] <= 0.0f) {
                explosions[i]->setPosition(randomExplosionPosition());
                explosions[i]->restart(true);
                explosionMoveTimers[i] = explosionCycleDuration;
            }
        }
    }

    void WinnerScene::initExplosion() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("explosion.png");
        material->set_mode(Stretched);
        material->set_spawn_shape(2);
        material->set_respawn_mode(0);
        material->set_life_min(0.6f);
        material->set_life_max(0.6f);
        material->set_size_max(0.01f);
        material->set_size_min(0.002f);
        material->set_stretch(0.0f);
        material->set_vel_min({ 0.5f, 0.0f, 0.0f });
        material->set_vel_max({ 1.3f, 0.0f, 0.0f });
        material->set_gravity({0.0f, 0.0f, -0.9f});
        material->set_emitter_radius(0.05f);
        material->set_color_start({8.0f, 4.0f, 1.0f, 1.0f});
        material->set_color_end({0.1f, 0.1f, 0.1f, 0.0f});

        explosions.clear();
        explosionMoveTimers.clear();
        explosions.reserve(kExplosionCount);
        explosionMoveTimers.reserve(kExplosionCount);

        explosionCycleDuration = std::max(0.1f, (material->get_life_max() / kTimeScale) - kResetLead);

        for (int i = 0; i < kExplosionCount; ++i) {
            const float shaderTimeOffset = kTimeOffsetStep * static_cast<float>(i);
            auto explosion = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 500);
            explosion->setTimeOffset(shaderTimeOffset);
            explosion->setTimeScale(kTimeScale);
            explosion->setPosition(randomExplosionPosition());
            explosion->restart(true);

            explosionMoveTimers.push_back(explosionCycleDuration - shaderTimeOffset);
            explosions.push_back(explosion);
            addMeshNode3D(explosion);
        }
    }

    glm::vec3 WinnerScene::randomExplosionPosition() {
        const float x = glm::linearRand(kPlaneMinX, kPlaneMaxX);
        const float y = glm::linearRand(kPlaneMinY, kPlaneMaxY);
        return {x, y, kExplosionZ};
    }
} // Scenes
