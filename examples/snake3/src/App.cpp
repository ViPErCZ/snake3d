#include <chrono>
#include <nlohmann/json.hpp>
#include "App.h"
#include "Renderer/Opengl/Material/Feature/FogFeature.h"
#include "Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Renderer/Opengl/RenderStats.h"
#include "Resource/AnimLoader.h"
#include "Resource/ShaderLoader.h"
#include "Resource/TextureLoader.h"

using namespace Animation;

App::App(const shared_ptr<Camera> &camera, const int width, const int height) : camera(camera), width(width), height(height) {
    resourceManager = make_shared<ResourceManager>();
    shaderRegistry = make_shared<ShaderRegistry>();
    resourceManager->setShaderRegistry(shaderRegistry);
    resourceManager->setFogFeature(make_shared<Feature::FogFeature>(false));
    keyboardManager = make_unique<KeyboardManager>();

    projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );
    contextState = make_shared<ContextState>();

    rendererManager = make_shared<RenderManager>(contextState, camera, resourceManager, projection, width, height);
    rendererManager->setWidth(width);
    rendererManager->setHeight(height);
    environment = make_shared<Environment>();
    mainScene = make_unique<MainScene>(
        nullptr,
        vector<shared_ptr<SpotLight> >{},
        vector<shared_ptr<PointLight> >{},
        rendererManager, camera, projection, resourceManager, width, height
    );
    //mainScene->setEnvironment(environment);
}

void App::initScene() const {
    mainScene->init(100);
    mainScene->attachRenderer();
}

void App::Init() {
    InitResourceManager();

    shaderRegistry->registerMaster("blur",
        "Assets/Shaders/bloom/blur.vs", "Assets/Shaders/bloom/blur.fs");
    shaderRegistry->registerMaster("bloomFinal",
        "Assets/Shaders/bloom/bloom_final.vs", "Assets/Shaders/bloom/bloom_final.fs");
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

    resourceManager->addShader("blur",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/blur.vs",
                "Assets/Shaders/bloom/blur.fs"
                ))
    );

    resourceManager->addShader(
        "bloomFinal",
        std::make_shared<ShaderProgram>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/bloom_final.vs",
                "Assets/Shaders/bloom/bloom_final.fs"
                ))
    );

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

    const auto warmup = shaderRegistry->warmupAll();
    std::cout << "[App] Shader pre-flight: " << warmup.compiled
              << " compiled, " << warmup.failed.size() << " failed\n";
    if (!warmup.failed.empty()) {
        std::cerr << "[App] FATAL: pre-flight failed for masters:";
        for (const auto& name : warmup.failed) std::cerr << " " << name;
        std::cerr << "\n";
        std::abort();
    }

    rendererManager->initBloom();
    rendererManager->initShadowMapping();
    rendererManager->initReflection();

    preloaderScene = make_shared<PreloaderScene>(nullptr,
        vector<shared_ptr<SpotLight> >{},
        vector<shared_ptr<PointLight> >{},
        rendererManager, camera, projection, resourceManager, width, height);
    preloaderScene->init(0);
    preloaderScene->attachRenderer();

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

void App::initDebugOverlay(GLFWwindow* window) {
    if constexpr (isDebug) {
        imguiOverlay = std::make_unique<ImGuiOverlay>(window, rendererManager);
    }
}

void App::run() {
    if (state == SceneState::LOADING) {
        resourceManager->processPending();

        if (!scanning && resourceManager->isAllLoaded()) {
            std::cout << "\rLoading DONE!      " << std::endl;
            state = SceneState::RUNNING;

            rendererManager->reset();
            initScene();
            if constexpr (isDebug) {
                if (imguiOverlay && mainScene) {
                    imguiOverlay->setManipulatorHandler(mainScene->getManipulatorHandler());
                    imguiOverlay->setScene(mainScene);
                }
            }
        }
    }

    if constexpr (isDebug) {
        if (imguiOverlay) imguiOverlay->beginFrame();
    }

    if (state == SceneState::RUNNING) {
        using clock = std::chrono::steady_clock;
        auto t0 = clock::now();
        mainScene->update();
        auto t1 = clock::now();
        mainScene->physics();
        auto t2 = clock::now();
        mainScene->render();
        auto t3 = clock::now();
        Renderer::RenderStats::updateMs  = std::chrono::duration<float, std::milli>(t1 - t0).count();
        Renderer::RenderStats::physicsMs = std::chrono::duration<float, std::milli>(t2 - t1).count();
        Renderer::RenderStats::renderMs  = std::chrono::duration<float, std::milli>(t3 - t2).count();
    } else {
        preloaderScene->update();
        preloaderScene->render();
    }

    if constexpr (isDebug) {
        if (imguiOverlay) {
            imguiOverlay->renderPanels();
            imguiOverlay->endFrame();
        }
    }
}

void App::processInput(GLFWwindow *window, const int keyCode, const int scancode, const int action, const int mods) const {
    mainScene->keyboardInput(window, keyCode, scancode, action, mods);
    //keyboardManager->onKeyPress(keyCode, scancode, action, mods);
}

void App::mouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods) const {
    if (mainScene) {
        mainScene->mouseButtonCallback(window, button, action, mods);
    }

    if (camera) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            // Disable the cursor while holding RMB so the spectator camera gets
            // unbounded mouse deltas - otherwise the cursor hits the screen edge
            // and yaw stops accumulating after ~half a turn.
            if (action == GLFW_PRESS) {
                cursorModeBeforeSpectator = glfwGetInputMode(window, GLFW_CURSOR);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, cursorModeBeforeSpectator);
            }
            camera->onMouseDown(button, action, mods);
        }
    }
}

void App::mousePositionCallback(GLFWwindow *window, const double x, const double y) const {
    const glm::vec2 cursor(static_cast<float>(x), static_cast<float>(y));

    if (mainScene) {
        mainScene->setCursorPosition(cursor);
    }
    if (state != SceneState::RUNNING || camera == nullptr || (mainScene && mainScene->isMenuVisible())) {
        return;
    }

    if constexpr (isDebug) {
        const bool ctrlHeld = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
                           || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
        const bool rmbHeld = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        if (!ctrlHeld && !rmbHeld) {
            lastMouseRotationSkipped = true;
            return;
        }

        if (lastMouseRotationSkipped) {
            camera->resetMouseDelta();
            lastMouseRotationSkipped = false;
        }
    }

    camera->processMouseMovement(x, y);
}

void App::setKeyState(const int key, const bool pressed) const {
    camera->setKeyState(key, pressed);
}

void App::cameraProcessKeyboard(GLFWwindow *window) const {
    if (mainScene && mainScene->isMenuVisible()) {
        return;
    }
    camera->processKeyboard(window, 1);
}

void App::resize(const int width, const int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    this->width = width;
    this->height = height;
    projection = glm::perspective(
        glm::radians(camera->getZoom()),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        1000.0f
    );

    if (rendererManager) {
        rendererManager->resize(width, height, projection);
    }

    if (mainScene) {
        mainScene->resize(width, height, projection);
    }

    if (preloaderScene) {
        preloaderScene->resize(width, height, projection);
    }
}

void App::InitResourceManager() const {
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
}
