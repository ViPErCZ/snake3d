#include <nlohmann/json.hpp>
#include "App.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Resource/AnimLoader.h"
#include "Resource/ShaderLoader.h"
#include "Resource/TextureLoader.h"

App::App(const shared_ptr<Camera> &camera, const int width, const int height) : camera(camera), width(width), height(height) {
    resourceManager = make_shared<ResourceManager>();
    shaderRegistry = make_shared<ShaderRegistry>();
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

    // B1: paralelní registrace masters v ShaderRegistry. GL kompilace je
    // lazy v get(), takže registrace samotná je čistě metadata a nezpůsobí
    // dvojí kompilaci.
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
    shaderRegistry->registerMaster("planeShader",
        "Assets/Shaders/basic.vs", "Assets/Shaders/plane.fs");
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
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/blur.vs",
                "Assets/Shaders/bloom/blur.fs"
                ))
    );

    resourceManager->addShader(
        "bloomFinal",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/bloom/bloom_final.vs",
                "Assets/Shaders/bloom/bloom_final.fs"
                ))
    );

    resourceManager->addShader(
        "shadowShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map.vs",
                "Assets/Shaders/shadow_map.fs"
            ))
    );
    resourceManager->addShader(
        "shadowDepthShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/shadow_map_depth.vs",
                "Assets/Shaders/shadow_map_depth.fs"
            ))
    );
    // B3a: basicShader je legacy materiál - kompiluje basic.fs se VŠEMI
    // features aktivními, takže nový `#ifdef FEATURE_*` blok se chová jako
    // dříve. Až materiály v B5+ začnou kompilovat skrze registry s
    // upraveným feature mask, tahle paralelní cesta zmizí.
    constexpr ShaderFeatureMask legacyBasicFeatures =
        ShaderFeature::PBR | ShaderFeature::NormalMap | ShaderFeature::Shadows |
        ShaderFeature::DirectionalLight | ShaderFeature::Fog | ShaderFeature::IBL;
    resourceManager->addShader(
        "basicShader",
        shaderRegistry->get({"basicShader", legacyBasicFeatures})
    );
    resourceManager->addShader(
        "planeShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/basic.vs",
                "Assets/Shaders/plane.fs"
            ))
    );
    resourceManager->addShader(
        "arrowGizmo",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/gizmo/arrow.vert",
                "Assets/Shaders/gizmo/arrow.frag"
            ))
    );
    resourceManager->addShader(
        "preloadShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/preloader/dots/dots.vs",
                "Assets/Shaders/preloader/dots/dots.fs"
            ))
    );
    resourceManager->addShader(
        "preloadShader2",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/preloader/dots2/dots2.vs",
                "Assets/Shaders/preloader/dots2/dots2.fs"
            ))
    );
    resourceManager->addShader(
        "particle_update",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_update.vs"
                ))
    );
    resourceManager->addShader(
        "particle_update_2d",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_update.vs"
                ))
    );
    resourceManager->addShader(
        "particle_3d_render",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_3d_render.vs",
                "Assets/Shaders/particle/particle_3d_render.fs"
                ))
    );
    resourceManager->addShader(
        "particle_render_2d",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_render_2d.vs",
                "Assets/Shaders/particle/particle_render_2d.fs"
                ))
    );
    resourceManager->addShader(
        "particle_3d_render_tex",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_3d_render_tex.vs",
                "Assets/Shaders/particle/particle_3d_render_tex.fs"
            ))
    );
    resourceManager->addShader(
        "particle_render_2d_tex",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/particle/particle_render_2d_tex.vs",
                "Assets/Shaders/particle/particle_render_2d_tex.fs"
            ))
    );

#ifdef IS_DEBUG
    // B2 smoke test: ověř že registry zkompiluje shader, cachuje, a že
    // permutation klíč rozlišuje různé feature masky. Zkompiluje basicShader
    // dvakrát navíc (0 + PBR|Shadows) - dočasné, padne v B6 kdy registry
    // převezme roli single source of truth a addShader cesta zmizí.
    {
        const auto p0a = shaderRegistry->get({"basicShader", 0});
        const auto p0b = shaderRegistry->get({"basicShader", 0});
        if (!p0a || p0a != p0b) {
            std::cerr << "[ShaderRegistry] smoke test FAIL - basicShader|0 cache miss\n";
        } else {
            std::cout << "[ShaderRegistry] basicShader|0 ok (id=" << p0a->getId() << ")\n";
        }

        const ShaderFeatureMask mask = ShaderFeature::PBR | ShaderFeature::Shadows;
        const auto pMasked = shaderRegistry->get({"basicShader", mask});
        if (!pMasked) {
            std::cerr << "[ShaderRegistry] smoke test FAIL - basicShader|PBR|Shadows compile failed\n";
        } else if (pMasked == p0a) {
            std::cerr << "[ShaderRegistry] smoke test FAIL - permutation cache collision\n";
        } else {
            std::cout << "[ShaderRegistry] basicShader|PBR|Shadows ok (id=" << pMasked->getId() << ")\n";
        }
    }
#endif

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

void App::run() {
    if (state == SceneState::LOADING) {
        resourceManager->processPending();

        if (!scanning && resourceManager->isAllLoaded()) {
            std::cout << "\rLoading DONE!      " << std::endl;
            state = SceneState::RUNNING;

            rendererManager->reset();
            initScene();
        }
    }

    if (state == SceneState::RUNNING) {
        mainScene->update();
        mainScene->physics();
        mainScene->render();
    } else {
        preloaderScene->update();
        preloaderScene->render();
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
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            } else if (action == GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
    if (state == SceneState::RUNNING && camera != nullptr && (!mainScene || !mainScene->isMenuVisible())) {
        camera->processMouseMovement(x, y);
    }
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

    resourceManager->loadAsyncShader("rain", "Assets/Shaders/rain/rain.vs", "", "Assets/Shaders/rain/rain.fs", []() {
        std::cout << "Shader rain ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("rainDrop", "Assets/Shaders/basic.vs", "", "Assets/Shaders/rain/raindrop.fs", []() {
        std::cout << "Shader rainDrop ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("fire", "Assets/Shaders/fire/fire.vs", "", "Assets/Shaders/fire/fire.fs", []() {
        std::cout << "Shader fire ready!" << std::endl;
    });

    resourceManager->loadAsyncShader("smoke", "Assets/Shaders/fire/smoke.vs", "", "Assets/Shaders/fire/smoke.fs", []() {
        std::cout << "Shader smoke ready!" << std::endl;
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
