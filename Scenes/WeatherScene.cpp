#include "WeatherScene.h"

#include "../Renderer/Opengl/Model/Standard/GPUParticle3D.h"
#include "../Renderer/Opengl/Model/Standard/QuadMesh3D.h"
#include "../Renderer/Opengl/Model/Standard/2D/GPUParticle2D.h"
#include "../Renderer/Opengl/Model/Standard/2D/QuadNode2D.h"

namespace Scenes {
    WeatherScene::WeatherScene(const shared_ptr<RenderManager> &rendererManager, const shared_ptr<Camera> &camera, const glm::mat4 &projection,
        const shared_ptr<ResourceManager> &rm, const int width, const int height)
        : Scene(rendererManager, camera, projection, rm, width, height) {
    }

    void WeatherScene::init(const int priority) {
        Scene::init(priority);

        quad = make_shared<QuadMesh3D>(resourceManager->getShader("basicShader"), 1.7, 1.7);
        quad->setBlending(Blending::AlphaAdditive);
        quad->setDepthTest(false);
        quad->setDepthWrite(false);

        initRain();
        initRainDrop();
        // initSnow();
        initExplosion();
    }

    void WeatherScene::initRain() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("rain.png");
        material->set_mode(Billboard);
        material->set_spawn_shape(1);
        material->set_respawn_mode(1);
        material->set_turbulence({0.0f, 0.0f});
        material->set_min_radius(3.0f);
        material->set_max_radius(30.0f);
        material->set_spawn_height(25.0f);
        material->set_color_start({0.25f, 0.35f, 0.8f, 0.45f});
        material->set_color_end({0.25f, 0.35f, 0.8f, 0.45f});
        material->set_color_sensitivity(26.0f);
        material->set_gravity({0.0f, 0.0f, -2.8f});
        material->set_emitter_radius(25.0f);
        material->set_emitter_y_offset(15.0f);
        material->set_vel_min({-0.2f, -0.2f, -15.0f});
        material->set_vel_max({0.2f, 0.2f, -25.0f});
        material->set_life_min(1.0f);
        material->set_life_max(1.8f);
        material->set_size_min(0.012f);
        material->set_size_max(0.012f);
        material->set_stretch(0.003f);

        const auto rain = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 6000);

        addMeshNode3D(rain);
    }

    void WeatherScene::initRainDrop() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("drop_normal.png");
        material->set_spawn_shape(1);
        material->set_respawn_mode(1);
        material->set_turbulence({1.0f, 0.0f});
        material->set_gravity({0.0f, -0.5f, 0.0f});
        material->set_vel_min({0.0f, -0.1f, 0.0f});
        material->set_vel_max({0.0f, -0.8f, 0.0f});
        material->set_life_min(1.0f);
        material->set_life_max(5.0f);
        material->set_size_min(0.009f);
        material->set_size_max(0.03f);
        material->set_color_start({0.8f, 0.9f, 1.0f, 0.3f});
        material->set_color_end({0.8f, 0.9f, 1.0f, 0.0f});
        material->set_color_sensitivity(1.0f);
        material->set_drag(0.5f);
        material->set_emitter_size({0.9f, 1.2f});
        material->set_spawn_per_frame(1.0f);

        const auto quad2D = make_shared<QuadNode2D>(0.9, 1.2);
        const auto rainDrop2D = make_shared<GPUParticle2D>(material, contextState, quad2D, resourceManager, 5);

        addMeshNode2D(rainDrop2D, 1);
    }

    void WeatherScene::initSnow() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("snow.png");
        material->set_mode(Billboard);
        material->set_spawn_shape(1);
        material->set_respawn_mode(1);
        material->set_turbulence({0.5f, 0.8f});
        material->set_min_radius(4.0f);
        material->set_max_radius(25.0f);
        material->set_spawn_height(20.0f);
        material->set_color_start({0.8f, 0.9f, 1.0f, 0.3f});
        material->set_color_end({0.8f, 0.9f, 1.0f, 0.0f});
        material->set_color_sensitivity(2.0f);
        material->set_gravity({0.0f, -0.5f, 0.0f});
        material->set_emitter_radius(30.0f);
        material->set_emitter_y_offset(10.0f);
        material->set_vel_min({-1.5f, -1.5f, -0.8f});
        material->set_vel_max({1.5f, 1.5f, -1.8f});
        material->set_life_min(1.0f);
        material->set_life_max(5.0f);
        material->set_size_min(0.009f);
        material->set_size_max(0.03f);
        material->set_stretch(0.0f);
        material->set_spawn_per_frame(0);

        const auto snow = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 6000);

        addMeshNode3D(snow);
    }

    void WeatherScene::initExplosion() {
        const auto material = make_shared<ParticleProcessMaterial>(resourceManager);
        material->set_texture("explosion.png");
        material->set_mode(Stretched);
        material->set_spawn_shape(2);
        material->set_respawn_mode(0);
        material->set_life_min(0.1f);
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

        const auto explosion1 = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 500);
        const auto explosion2 = make_shared<GPUParticle3D>(material, contextState, camera, quad, resourceManager, 500);

        explosion1->setPosition(glm::vec3(0.0, 0.6, 0.0));
        explosion2->setPosition(glm::vec3(2.0, 0.6, 0.0));
        explosion2->setTimeOffset(0.2f);

        addMeshNode3D(explosion1);
        addMeshNode3D(explosion2);
    }
} // Scenes