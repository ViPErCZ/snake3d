#include "Snake3App.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

#include <stdafx.h>

#include <snake3d/Manager/ShaderRegistry.h>
#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Manager/TextureManager.h>
#include <snake3d/Renderer/Opengl/Material/Feature/FogFeature.h>
#include <snake3d/Renderer/Opengl/Material/Uniform/TextureArrayUniform.h>
#include <snake3d/Renderer/Opengl/Model/Standard/AnimationArrayMesh.h>
#include <snake3d/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Mesh.h>
#include <snake3d/Renderer/Opengl/RenderStats.h>
#include <snake3d/Resource/AnimLoader.h>
#include <snake3d/Resource/ShaderLoader.h>
#include <snake3d/Resource/TextureLoader.h>
#include <snake3d/Tools/BuildSettings.h>

#include "Scenes/MainScene.h"
#include "Scenes/PreloaderScene.h"

namespace fs = std::filesystem;

using namespace std;
using namespace snake3d;
using namespace Animation;
using namespace Build;
using namespace Handler::Debug;
using namespace Manager;
using namespace Model;
using namespace Renderer;
using namespace Resource;
using namespace Scenes;

namespace {
    struct TextureEntry {
        std::string name;
        std::string path;
        std::string category;
    };
}

Snake3App::Snake3App() = default;
Snake3App::~Snake3App() = default;

WindowConfig Snake3App::getWindowConfig() const {
    WindowConfig cfg;
    cfg.title = "Snake 3";
    cfg.width = 1920;
    cfg.height = 1080;
    cfg.glMajor = 3;
    cfg.glMinor = 3;
    cfg.compatProfile = true;
    return cfg;
}

void Snake3App::onInit() {
    // Build mainScene now that all managers are alive. Construction is cheap;
    // init() runs later (after assets are ready) in onSceneReady().
    mainScene = make_shared<MainScene>(
        nullptr,
        vector<shared_ptr<Lights::SpotLight>>{},
        vector<shared_ptr<Lights::PointLight>>{},
        rendererManager, camera, projection, resourceManager, width, height
    );

    registerShaders();
    preflightShaders();

    rendererManager->initBloom();
    rendererManager->initShadowMapping();
    rendererManager->initReflection();

    buildPreloaderScene();
    initResourceManifest();
}

void Snake3App::onSceneReady() {
    mainScene->init(100);
    mainScene->attachRenderer();
    wireDebugOverlay();
}

void Snake3App::onFrame(float /*dt*/) {
    using clock = std::chrono::steady_clock;
    auto t0 = clock::now();
    mainScene->update();
    auto t1 = clock::now();
    mainScene->physics();
    auto t2 = clock::now();
    mainScene->render();
    auto t3 = clock::now();
    RenderStats::updateMs  = std::chrono::duration<float, std::milli>(t1 - t0).count();
    RenderStats::physicsMs = std::chrono::duration<float, std::milli>(t2 - t1).count();
    RenderStats::renderMs  = std::chrono::duration<float, std::milli>(t3 - t2).count();
}

void Snake3App::onLoadingFrame() {
    if (preloaderScene) {
        preloaderScene->update();
        preloaderScene->render();
    }
}

void Snake3App::onKeyboardInput(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) {
    if (mainScene) {
        mainScene->keyboardInput(window, key, scancode, action, mods);
    }
}

void Snake3App::onMouseButton(GLFWwindow* window, const int button, const int action, const int mods) {
    if (mainScene) {
        mainScene->mouseButtonCallback(window, button, action, mods);
    }
}

void Snake3App::onCursorPos(GLFWwindow* /*window*/, const double x, const double y) {
    const glm::vec2 cursor(static_cast<float>(x), static_cast<float>(y));
    if (mainScene) {
        mainScene->setCursorPosition(cursor);
    }
}

void Snake3App::onResize(const int w, const int h, const glm::mat4& proj) {
    if (mainScene) {
        mainScene->resize(w, h, proj);
    }
    if (preloaderScene) {
        preloaderScene->resize(w, h, proj);
    }
}

bool Snake3App::shouldProcessCameraKeyboard() const {
    return !(mainScene && mainScene->isMenuVisible());
}

bool Snake3App::shouldProcessCameraMouse() const {
    return !(mainScene && mainScene->isMenuVisible());
}

void Snake3App::registerShaders() {
    // Bloom shaders are engine-owned now: RenderManager::initBloom() self-registers
    // "blur" + "bloomFinal" from EngineShaders/bloom (no per-example copy/register).
    shaderRegistry->registerMaster("shadowShader",
        "Assets/Shaders/shadow_map.vs", "Assets/Shaders/shadow_map.fs");
    shaderRegistry->registerMaster("shadowDepthShader",
        "Assets/Shaders/shadow_map_depth.vs", "Assets/Shaders/shadow_map_depth.fs");
    shaderRegistry->registerMaster("basicShader",
        "Assets/Shaders/basic.vs", "Assets/Shaders/basic.fs");
    shaderRegistry->registerMaster("arrowGizmo",
        "Assets/Shaders/gizmo/arrow.vert", "Assets/Shaders/gizmo/arrow.frag");
    shaderRegistry->registerMaster("preloadShader",
        "Assets/Shaders/preloader/dots/dots.vs", "Assets/Shaders/preloader/dots/dots.fs");
    shaderRegistry->registerMaster("preloadShader2",
        "Assets/Shaders/preloader/dots2/dots2.vs", "Assets/Shaders/preloader/dots2/dots2.fs");
    shaderRegistry->registerMasterVertexOnly("particle_update",
        "Assets/Shaders/particle/particle_update.vs");
    shaderRegistry->registerMasterVertexOnly("particle_update_2d",
        "Assets/Shaders/particle/particle_update.vs");
    shaderRegistry->registerMaster("particle_3d_render",
        "Assets/Shaders/particle/particle_3d_render.vs", "Assets/Shaders/particle/particle_3d_render.fs");
    shaderRegistry->registerMaster("particle_render_2d",
        "Assets/Shaders/particle/particle_render_2d.vs", "Assets/Shaders/particle/particle_render_2d.fs");
    shaderRegistry->registerMaster("particle_3d_render_tex",
        "Assets/Shaders/particle/particle_3d_render_tex.vs", "Assets/Shaders/particle/particle_3d_render_tex.fs");
    shaderRegistry->registerMaster("particle_render_2d_tex",
        "Assets/Shaders/particle/particle_render_2d_tex.vs", "Assets/Shaders/particle/particle_render_2d_tex.fs");

    resourceManager->addShader(
        "shadowShader",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map.vs",
                "Assets/Shaders/shadow_map.fs"
            ))
    );
    resourceManager->addShader(
        "shadowDepthShader",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map_depth.vs",
                "Assets/Shaders/shadow_map_depth.fs"
            ))
    );
    constexpr ShaderFeatureMask legacyBasicFeatures =
        ShaderFeature::PBR | ShaderFeature::NormalMap | ShaderFeature::Shadows |
        ShaderFeature::DirectionalLight | ShaderFeature::Fog | ShaderFeature::IBL |
        ShaderFeature::Bones;
    resourceManager->addShader(
        "basicShader",
        shaderRegistry->get({"basicShader", legacyBasicFeatures})
    );
    resourceManager->addShader(
        "arrowGizmo",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/gizmo/arrow.vert",
                "Assets/Shaders/gizmo/arrow.frag"
            ))
    );
    resourceManager->addShader(
        "preloadShader",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/preloader/dots/dots.vs",
                "Assets/Shaders/preloader/dots/dots.fs"
            ))
    );
    resourceManager->addShader(
        "preloadShader2",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/preloader/dots2/dots2.vs",
                "Assets/Shaders/preloader/dots2/dots2.fs"
            ))
    );
    resourceManager->addShader(
        "particle_update",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_update.vs"
                ))
    );
    resourceManager->addShader(
        "particle_update_2d",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_update.vs"
                ))
    );
    resourceManager->addShader(
        "particle_3d_render",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_3d_render.vs",
                "Assets/Shaders/particle/particle_3d_render.fs"
                ))
    );
    resourceManager->addShader(
        "particle_render_2d",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_render_2d.vs",
                "Assets/Shaders/particle/particle_render_2d.fs"
                ))
    );
    resourceManager->addShader(
        "particle_3d_render_tex",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_3d_render_tex.vs",
                "Assets/Shaders/particle/particle_3d_render_tex.fs"
            ))
    );
    resourceManager->addShader(
        "particle_render_2d_tex",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_render_2d_tex.vs",
                "Assets/Shaders/particle/particle_render_2d_tex.fs"
            ))
    );
}

void Snake3App::preflightShaders() {
    const auto warmup = shaderRegistry->warmupAll();
    std::cout << "[Snake3App] Shader pre-flight: " << warmup.compiled
              << " compiled, " << warmup.failed.size() << " failed\n";
    if (!warmup.failed.empty()) {
        std::cerr << "[Snake3App] FATAL: pre-flight failed for masters:";
        for (const auto& name : warmup.failed) std::cerr << " " << name;
        std::cerr << "\n";
        std::abort();
    }
}

void Snake3App::buildPreloaderScene() {
    preloaderScene = make_shared<PreloaderScene>(nullptr,
        vector<shared_ptr<Lights::SpotLight>>{},
        vector<shared_ptr<Lights::PointLight>>{},
        rendererManager, camera, projection, resourceManager, width, height);
    preloaderScene->init(0);
    preloaderScene->attachRenderer();
}

void Snake3App::wireDebugOverlay() {
    if constexpr (isDebug) {
        if (imguiOverlay && mainScene) {
            imguiOverlay->setManipulatorHandler(mainScene->getManipulatorHandler());
            imguiOverlay->setScene(mainScene);
            imguiOverlay->setCollisionSystem(mainScene->getCollisionSystem());
        }
    }
}

void Snake3App::initResourceManifest() {
    vector<string> faces;
    faces.emplace_back("Assets/Skybox/cloud/right.jpg");
    faces.emplace_back("Assets/Skybox/cloud/left.jpg");
    faces.emplace_back("Assets/Skybox/cloud/top.jpg");
    faces.emplace_back("Assets/Skybox/cloud/bottom.jpg");
    faces.emplace_back("Assets/Skybox/cloud/front.jpg");
    faces.emplace_back("Assets/Skybox/cloud/back.jpg");
    const unsigned int cubeMapTexture = TextureLoader::loadSkyboxTexture(faces);
    const auto texture = std::make_shared<TextureManager>();
    texture->addTexture(cubeMapTexture);
    resourceManager->addTexture("skybox", texture);

    std::ifstream manifestFile("Assets/texture_manifest.json");
    if (!manifestFile.is_open()) {
        throw std::runtime_error("Cant open manifest file.");
    }

    nlohmann::json j;
    manifestFile >> j;

    std::vector<TextureEntry> textures;

    for (auto &item : j) {
        cout << "Loading texture: " << item["name"] << endl;
        textures.push_back(TextureEntry{
            item["name"].get<std::string>(),
            item["path"].get<std::string>(),
            item["category"].get<std::string>()
        });
    }

    for (const auto &[name, path, category] : textures) {
        bool isAlbedo = (category == "Albedo");

        resourceManager->loadAsyncTexture("Assets/Textures/" + path, name, isAlbedo, [name=name]() {
            std::cout << "Texture ready: " << name << std::endl;
        });
    }

    resourceManager->loadAsyncShader("textShader", "Assets/Shaders/text.vs", "", "Assets/Shaders/text.fs", []() {
        std::cout << "Shader textShader ready!" << std::endl;
    });
    resourceManager->loadAsyncShader("textTitle", "Assets/Shaders/text_title_fx.vs", "",
        "Assets/Shaders/text_title_fx.fs", []() {
        std::cout << "Shader textTitle ready!" << std::endl;
    });
    resourceManager->loadAsyncShader("titleImageFx", "Assets/Shaders/basic_2d.vs", "",
        "Assets/Shaders/title_image_fx.fs", []() {
        std::cout << "Shader titleImageFx ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("gizmoShader",
                                       "Assets/Shaders/gizmo/gizmo.vs",
                                       "Assets/Shaders/gizmo/gizmo.geom",
                                       "Assets/Shaders/gizmo/gizmo.fs",
                                       []() {
        std::cout << "Shader gizmo ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("markRingShader",
        "Assets/Shaders/ring/ring.vs", "",
        "Assets/Shaders/ring/ring.fs", []() {
        std::cout << "Shader markRingShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("respawnShader", "Assets/Shaders/basic.vs", "", "Assets/Shaders/respawn/respawn.fs", []() {;
        std::cout << "Shader respawnShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("cursor2d", "Assets/Shaders/basic_2d.vs", "", "Assets/Shaders/cursor_2d.fs", []() {
        std::cout << "Shader cursor2d ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("particle_render_2d_trail", "Assets/Shaders/particle/particle_render_2d.vs", "",
        "Assets/Shaders/particle/particle_render_2d_trail.fs", []() {
        std::cout << "Shader particle_render_2d_trail ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("normalShader", "Assets/Shaders/normal_map.vs", "", "Assets/Shaders/normal_map.fs", []() {
        std::cout << "Shader normalShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("basic2d", "Assets/Shaders/basic_2d.vs", "", "Assets/Shaders/basic_2d.fs", []() {;
        std::cout << "Shader basic2d ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("quadCorner", "Assets/Shaders/basic_2d.vs", "", "Assets/Shaders/corner/corner.fs", []() {;
        std::cout << "Shader quadCorner ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("skyboxShader", "Assets/Shaders/skybox.vs", "", "Assets/Shaders/skybox.fs", []() {
        std::cout << "Shader skyboxShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("debugQuadShader", "Assets/Shaders/debug_quad.vs", "", "Assets/Shaders/debug_quad.fs", []() {
        std::cout << "Shader debugQuadShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("boltShader", "Assets/Shaders/bolt/bolt.vs", "", "Assets/Shaders/bolt/bolt.fs", []() {
        std::cout << "Shader boltShader ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("flash", "Assets/Shaders/bolt/flash.vs", "", "Assets/Shaders/bolt/flash.fs", []() {
        std::cout << "Shader flash ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("explosion",
                                       "Assets/Shaders/explosion/explosion.vs",
                                       "Assets/Shaders/explosion/explosion.geom",
                                       "Assets/Shaders/explosion/explosion.fs", []() {
        std::cout << "Shader explosion ready!" << std::endl;
    });

    const fs::path assets_dir{"Assets/Objects"};
    resourceManager->loadAsyncModel<AnimationPlayer>(assets_dir / "pacman.glb", "pacman", []() {
        std::cout << "Model pacman ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<Mesh>(assets_dir / "Coin.obj", "coin", []() {
        std::cout << "Model coin ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<Mesh>(assets_dir / "torch.glb", "torch", []() {
        std::cout << "Model torch ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<Mesh>(assets_dir / "streetlamp.glb", "streetlamp", []() {
        std::cout << "Model street lamp ready!" << std::endl;
    });
    resourceManager->loadAsyncModel<Mesh>(assets_dir / "barrel.glb", "barrel", []() {
        std::cout << "Model barrel ready!" << std::endl;
    });
}
