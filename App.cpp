#include <nlohmann/json.hpp>
#include "App.h"
#include "Renderer/Opengl/BoltRenderer.h"
#include "Renderer/Opengl/Material/StandardMaterial.h"
#include "Renderer/Opengl/Material/Uniform/TextureArrayUniform.h"
#include "Renderer/Opengl/Model/Standard/AnimationArrayMesh.h"
#include "Resource/AnimLoader.h"
#include "Resource/ShaderLoader.h"
#include "Resource/TextureLoader.h"

App::App(const shared_ptr<Camera> &camera, const int width, const int height) : camera(camera), width(width), height(height) {
    resourceManager = make_shared<ResourceManager>();
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

void App::initScene() {
    mainScene->init(100);
    //const glm::mat4 ortho = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1000.0f);

    // auto basicShader = resourceManager->getShader("basicShader");
    // auto shadowDepthShader = resourceManager->getShader("shadowDepthShader");
    // const auto planeMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    // const auto coinMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    // const auto boxMaterial = make_shared<StandardMaterial>(StandardMaterial(basicShader, shadowDepthShader));
    //
    // auto gamefieldAlbedo = resourceManager->getTexture("gamefield.bmp");
    // auto gamefieldNormal = resourceManager->getTexture("gamefield_normal.jpg");
    // auto gamefieldSpecular = resourceManager->getTexture("gamefield_specular.jpg");
    // // auto skeletonAlbedo = resourceManager->getTexture("Skeleton_Body.png");
    // // auto skeletonORM = resourceManager->getTexture("Skeleton_Body_ORM.png");
    // auto shadowMap = resourceManager->getTexture("depth");
    // auto coinAlbedo = resourceManager->getTexture("Coin_Gold_albedo.png");
    // auto coinNormal = resourceManager->getTexture("Coin_Gold_nm.png");
    // auto coinMetalness = resourceManager->getTexture("Coin_Gold_metalness.png");
    // auto coinRoughness = resourceManager->getTexture("Coin_Gold_rough.png");
    // auto rustedAlbedo = resourceManager->getTexture("rusted_albedo.png");
    // auto rustedNormal = resourceManager->getTexture("rusted_normal.png");
    // auto rustedRoughness = resourceManager->getTexture("rusted_roughness.png");
    // auto aoMap = resourceManager->getTexture("ao.png");
    // auto rustedMetallic = resourceManager->getTexture("rusted_metallic.png");
    // auto brickWall = resourceManager->getTexture("brickwork-texture.jpg");
    // auto brickWallNormal = resourceManager->getTexture("brickwork_normal-map.jpg");
    // auto brickWallSpecular = resourceManager->getTexture("brickwork-bump-map.jpg");
    // auto environmentMap = resourceManager->getTexture("skybox");
    //
    // planeMaterial->setColor({0.88, 0.05, 0.05});
    // boxMaterial->setAlbedo(brickWall);
    // boxMaterial->setNormal(brickWallNormal);
    // boxMaterial->setSpecular(brickWallSpecular);
    // // planeMaterial->setAlbedo(gamefieldAlbedo);
    // coinMaterial->setAlbedo(coinAlbedo);
    // coinMaterial->setNormal(coinNormal);
    // coinMaterial->setSpecular(coinMetalness);
    // planeMaterial->setShadow(shadowMap);
    // coinMaterial->setShadow(shadowMap);
    // boxMaterial->setShadow(shadowMap);
    // // planeMaterial->setNormalEnabled(true);
    // coinMaterial->setNormalEnabled(true);
    // boxMaterial->setNormalEnabled(true);

}

void App::Init() {
    InitResourceManager();

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
    resourceManager->addShader(
        "basicShader",
        std::make_shared<ShaderManager>(
            ShaderLoader::loadShader(
                "Assets/Shaders/basic.vs",
                "Assets/Shaders/basic.fs"
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

    rendererManager->initBloom();
    rendererManager->initShadowMapping();
    rendererManager->initReflection();

    preloaderScene = make_shared<PreloaderScene>(nullptr,
        vector<shared_ptr<SpotLight> >{},
        vector<shared_ptr<PointLight> >{},
        rendererManager, camera, projection, resourceManager, width, height);
    preloaderScene->init(0);

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

    switch (keyCode) {
        case GLFW_KEY_ESCAPE:
            if (state == SceneState::RUNNING) {
                glfwSetWindowShouldClose(window, true);
            }
        default:
            break;
    }
}

void App::mouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods) const {
    double xpos, ypos;
    // glfwGetCursorPos(window, &xpos, &ypos);
    // const glm::vec2 cursor(static_cast<float>(xpos), static_cast<float>(ypos));

    // if (torchRenderer != nullptr) {
    //     if (button == GLFW_MOUSE_BUTTON_LEFT) {
    //         if (action == GLFW_PRESS) {
    //             torchRenderer->onMouseDown(cursor, width, height);
    //         } else if (action == GLFW_RELEASE) {
    //             torchRenderer->onMouseUp();
    //         }
    //     }
    // }
    if (camera) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            camera->onMouseDown(button, action, mods);
        }
    }
}

void App::mousePositionCallback(GLFWwindow *window, const double x, const double y) const {
    const glm::vec2 cursor(static_cast<float>(x), static_cast<float>(y));
    //if (torchRenderer != nullptr) {
    //    torchRenderer->onMouseMove(cursor, width, height);
    //}
    if (state == SceneState::RUNNING && camera != nullptr) {
        camera->processMouseMovement(x, y);
    }
}

void App::setKeyState(const int key, const bool pressed) const {
    camera->setKeyState(key, pressed);
}

void App::cameraProcessKeyboard(GLFWwindow *window) const {
    camera->processKeyboard(window, 1);
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
