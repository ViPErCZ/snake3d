#include <snake3d/Manager/RenderManager.h>
#include <algorithm>
#include <chrono>

#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Resource/ShaderLoader.h>
#include <snake3d/Renderer/Opengl/Material/Feature/FogFeature.h>
#include <snake3d/Renderer/Opengl/RenderStats.h>
#include <snake3d/Tools/Frustum.h>

using namespace std;
using namespace Renderer;
using namespace Tools;
using namespace Lights;

namespace Manager {
    RenderManager::RenderManager(const shared_ptr<ContextState> &contextState, const shared_ptr<Camera> &camera,
            const shared_ptr<ResourceManager> &resourceManager, const glm::mat4 &projection, const int width, const int height)
        : resourceManager(resourceManager), camera(camera), contextState(contextState), projection(projection), width(width), height(height),
          shadows(false), bloom(false), reflections(false), fog(false) {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
    }

    void RenderManager::populateAndUploadFrameUbo() {
        frameData.view = camera->getViewMatrix();
        frameData.projection = projection;
        frameData.viewPos = camera->getPosition();
        frameData.uTime = static_cast<float>(glfwGetTime());

        // D1.1c-fix: dirLight už není ve FrameData. Snake body/head si vozí
        // vlastní DirectionalLight (PlayerScene/RemoteSnakeScene) -- jeden
        // sdílený frame UBO by je přebil globálním (jasnějším) světlem.
        // Per-material dirLight je teď v MaterialData UBO, LightingFeature
        // / ShaderMaterial::bind ho tam zapisují.
        frameUbo.upload(frameData);
        frameUbo.bind();
    }

    shared_ptr<ContextState> RenderManager::getContextState() const {
        return contextState;
    }

    void RenderManager::initBloom() {
        // Self-register the engine bloom shaders from EngineShaders/bloom (dropped
        // next to the executable by the copy_engine_shaders target) so ANY example
        // gets bloom by just calling initBloom() + toggleBloom() — no per-example
        // shader copying or registration. The BloomRenderer ctor fetches them, so
        // they must exist BEFORE it is built.
        resourceManager->addShader("blur", std::make_shared<ShaderProgram>(
            Resource::ShaderLoader::loadShader(
                "EngineShaders/bloom/blur.vs", "EngineShaders/bloom/blur.fs")));
        resourceManager->addShader("bloomFinal", std::make_shared<ShaderProgram>(
            Resource::ShaderLoader::loadShader(
                "EngineShaders/bloom/bloom_final.vs", "EngineShaders/bloom/bloom_final.fs")));
        bloomRenderer = make_unique<BloomRenderer>(resourceManager, width, height);
    }

    void RenderManager::initShadowMapping(const int shadowResolution) {
        depthMapRenderer = make_unique<DepthMapRenderer>(camera.get(), projection, resourceManager.get(),
                                                         shadowResolution);
    }

    void RenderManager::initReflection() {
        planarReflectionRenderer = make_unique<PlanarReflectionRenderer>(contextState, resourceManager, camera, projection, width, height);
        planarReflectionRenderer->setRenderManager(this);
        planarReflectionRenderer->updateRenderers(renderers);
    }

    void RenderManager::initRefraction() {
        refractionRenderer = make_unique<Renderer::RefractionRenderer>(contextState, resourceManager, camera, projection, width, height);
        refractionRenderer->setRenderManager(this);
        refractionRenderer->updateRenderers(renderers);
    }

    void RenderManager::setReflectionPlane(const glm::vec3 &normal, const float offset) {
        if (planarReflectionRenderer) planarReflectionRenderer->setReflectionPlane(normal, offset);
    }

    void RenderManager::setReflectionResolutionScale(const float scale) {
        if (planarReflectionRenderer) planarReflectionRenderer->setResolutionScale(scale);
        if (refractionRenderer) refractionRenderer->setResolutionScale(scale);
    }

    void RenderManager::addRenderer(shared_ptr<BaseRenderer> renderer, const int priority) {
        renderer->setShadow(shadows);
        renderers.push_back({std::move(renderer), priority});
        ranges::stable_sort(renderers,
                            [](auto &a, auto &b) { return a.priority > b.priority; });
        if (planarReflectionRenderer) {
            planarReflectionRenderer->updateRenderers(renderers);
        }
        if (refractionRenderer) {
            refractionRenderer->updateRenderers(renderers);
        }
    }

    bool RenderManager::removeRenderer(const shared_ptr<BaseRenderer> &renderer) {
        const auto before = renderers.size();
        erase_if(renderers,
                 [&renderer](const RendererEntry &entry) { return entry.renderer == renderer; });
        if (planarReflectionRenderer) {
            planarReflectionRenderer->updateRenderers(renderers);
        }
        if (refractionRenderer) {
            refractionRenderer->updateRenderers(renderers);
        }
        return renderers.size() != before;
    }

    void RenderManager::render(const float dt) {
        // Per-frame stats reset - voláno hned na začátku, draw call counter
        // se inkrementuje v glDraw* sites, ImGui Engine panel čte lastFrame.
        RenderStats::newFrame();

        // Reset depth state in case a previous pass disabled depth writes/tests.
        contextState->setDepthWrite(true);
        contextState->setDepthTest(true);

        if (reflections && planarReflectionRenderer) {
            camera->syncFollowPosition();
            RenderStats::setPass(RenderPass::Reflection);
            planarReflectionRenderer->render3D(dt, gFrameId);
        }
        if (reflections && refractionRenderer) {
            camera->syncFollowPosition();
            RenderStats::setPass(RenderPass::Reflection);
            refractionRenderer->render3D(dt, gFrameId);
        }
        RenderStats::setPass(RenderPass::Main);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glEnable(GL_DEPTH_TEST);
        glLoadIdentity();
        if (!bloom) {
            glClearColor(.0, .0, .0, 1.0);
        } else {
            constexpr auto backgroundColor = glm::vec3(0.0, 0.0, 0.0);
            constexpr float backgroundIntensity = {2.0f};
            glClearColor(
                backgroundColor.r * backgroundIntensity,
                backgroundColor.g * backgroundIntensity,
                backgroundColor.b * backgroundIntensity,
                1.0f
            );
        }

        glViewport(0, 0, width, height);

        if (shadows && depthMapRenderer) {
            glDepthFunc(GL_LESS);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(shadowDepthBias, shadowDepthBias);

            glm::vec3 sceneMin(FLT_MAX);
            glm::vec3 sceneMax(-FLT_MAX);

            for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                sceneMin = Iter->renderer->compareSceneMin(sceneMin);
                sceneMax = Iter->renderer->compareSceneMax(sceneMax);
            }

            shared_ptr<DirectionalLight> light = directionalLight;
            if (directionalLight == nullptr) {
                light = make_shared<DirectionalLight>();
                light->setPosition({0.0f, 7.0f, 11.0f});
                light->setDirection({1, 1.0, -3});
            }
            const auto lightSpacesMatrix = depthMapRenderer->computeLightSpaceMatrix(light, sceneMin, sceneMax);
            int index = 0;

            RenderStats::setPass(RenderPass::Shadow);
            for (auto & matrix : lightSpacesMatrix) {
                depthMapRenderer->beforeRender(index);
                depthMapRenderer->bind(index, matrix);

                // Cull shadow casters to this cascade's light-space box: the matrix is
                // lightProjection*lightView, so its frustum bounds exactly the casters
                // that can project into this cascade. Off-box casters contribute no
                // shadow to the lit area, so skipping them is loss-free.
                const Tools::Frustum lightFrustum(matrix);
                Renderer::CullState::frustum = &lightFrustum;

                // Opt-in front-face culling for the depth pass eliminates self-shadow
                // acne on closed casters. RAW GL (not ContextState): the overlay / 2D
                // passes toggle GL_CULL_FACE outside ContextState, so its cache is stale
                // at shadow-pass entry and a cached enable() would no-op, leaving culling
                // off. Restored to the engine default (disabled) right after the casters.
                if (shadowFrontCull) {
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                }

                for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
                    if (Iter->renderer->isShadow()) {
                        Iter->renderer->renderShadowMap();
                    }
                }
                Renderer::CullState::frustum = nullptr;
                if (shadowFrontCull) {
                    glCullFace(GL_BACK);
                    glDisable(GL_CULL_FACE);
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                // reset viewport
                glViewport(0, 0, width, height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                depthMapRenderer->render(dt);

                index++;
            }

            glDisable(GL_POLYGON_OFFSET_FILL);
        }

        constexpr GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);

        if (bloom && bloomRenderer) {
            bloomRenderer->beforeRender(MODE::bloom);
        }

        RenderStats::setPass(RenderPass::Main);

        // CPU + GPU timing main pass. CPU-only = chrono mezi start a end loop.
        // CPU+GPU = chrono přes glFinish (forced sync). Rozdíl = pure GPU.
        const auto mainStart = std::chrono::steady_clock::now();
        populateAndUploadFrameUbo();
        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->beforeRender(standard);
            Iter->renderer->render3D(dt, gFrameId);
            Iter->renderer->afterRender();
        }

        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->beforeRender(standard);
            Iter->renderer->render2D(dt, gFrameId);
            Iter->renderer->afterRender();
        }
        const auto cpuEnd = std::chrono::steady_clock::now();
        glFinish();
        const auto gpuEnd = std::chrono::steady_clock::now();
        RenderStats::mainPassMsCpuOnly =
            std::chrono::duration<float, std::milli>(cpuEnd - mainStart).count();
        RenderStats::mainPassMsCpu =
            std::chrono::duration<float, std::milli>(gpuEnd - mainStart).count();

        if (bloom && bloomRenderer) {
            this->bloomRenderer->afterRender();
        }
        gFrameId++;
    }

    void RenderManager::setWidth(const int width) {
        RenderManager::width = width;
    }

    void RenderManager::setHeight(const int height) {
        RenderManager::height = height;
    }

    void RenderManager::setProjection(const glm::mat4 &projection) {
        this->projection = projection;
    }

    void RenderManager::resize(const int width, const int height, const glm::mat4 &projection) {
        this->width = width;
        this->height = height;
        this->projection = projection;

        if (bloomRenderer) {
            bloomRenderer->resize(width, height, projection);
        }
        if (planarReflectionRenderer) {
            planarReflectionRenderer->resize(width, height, projection);
        }
        if (refractionRenderer) {
            refractionRenderer->resize(width, height, projection);
        }

        for (auto &entry : renderers) {
            entry.renderer->resize(width, height, projection);
        }
    }

    void RenderManager::setDepthMapRenderer(unique_ptr<DepthMapRenderer> &depthMapRenderer) {
        RenderManager::depthMapRenderer = std::move(depthMapRenderer);
    }

    void RenderManager::enableShadows() {
        shadows = true;
        updateShadows();
    }

    void RenderManager::disableShadows() {
        shadows = false;
        updateShadows();
    }

    void RenderManager::updateShadows() {
        for (auto Iter = renderers.begin(); Iter < renderers.end(); ++Iter) {
            Iter->renderer->setShadow(shadows);
        }
    }

    void RenderManager::toggleShadows() {
        shadows = !shadows;
        updateShadows();
    }

    void RenderManager::setBloomRenderer(unique_ptr<BloomRenderer> &bloomRenderer) {
        RenderManager::bloomRenderer = std::move(bloomRenderer);
    }

    void RenderManager::setPlanarReflectionRenderer(unique_ptr<PlanarReflectionRenderer> planarReflectionRenderer) {
        RenderManager::planarReflectionRenderer = std::move(planarReflectionRenderer);
    }

    void RenderManager::toggleBloom() {
        bloom = !bloom;
    }

    void RenderManager::toggleFog() {
        fog = !fog;
        if (const auto fogFeature = resourceManager ? resourceManager->getFogFeature() : nullptr) {
            fogFeature->setEnabled(fog);
        }
    }

    void RenderManager::toggleReflections() {
        reflections = !reflections;
        if (reflectionsCallback) {
            reflectionsCallback(reflections);
        }
    }

    void RenderManager::setReflectionsCallback(ReflectionsToggleCallback cb) {
        reflectionsCallback = std::move(cb);
    }

    bool RenderManager::isReflectionsEnabled() const {
        return reflections;
    }

    bool RenderManager::isFogEnabled() const {
        return fog;
    }

    void RenderManager::reloadShaders() const {
        if (!resourceManager) return;
        const auto registry = resourceManager->getShaderRegistry();
    }

    void RenderManager::reset() {
        renderers.clear();
    }

    const vector<RendererEntry>& RenderManager::getRenderers() const {
        return renderers;
    }

    void RenderManager::updateDirectionalLight(const shared_ptr<DirectionalLight> &light) {
        directionalLight = light;
    }

}
