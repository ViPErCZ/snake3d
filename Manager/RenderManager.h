#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../Renderer/Opengl/BloomRenderer.h"
#include "../Renderer/Opengl/PlanarReflectionRenderer.h"
#include "FrameUbo.h"
#include <functional>
#include <vector>

namespace Manager {
    enum class RenderPassType {
        Shadow,
        Scene,
        PostProcess
    };

    class RenderManager final {
    public:
        RenderManager(const std::shared_ptr<Tools::ContextState> &contextState,
                      const std::shared_ptr<Camera> &camera, const std::shared_ptr<ResourceManager> &resourceManager,
                      const glm::mat4 &projection,
                      int width, int height);

        ~RenderManager() = default;

        [[nodiscard]] std::shared_ptr<Tools::ContextState> getContextState() const;

        void initBloom();

        void initShadowMapping();

        void initReflection();

        void render(float dt);

        void addRenderer(std::shared_ptr<Renderer::BaseRenderer> renderer, int priority = 0);
        bool removeRenderer(const std::shared_ptr<Renderer::BaseRenderer> &renderer);

        void setDepthMapRenderer(std::unique_ptr<Renderer::DepthMapRenderer> &depthMapRenderer);

        void setBloomRenderer(std::unique_ptr<Renderer::BloomRenderer> &bloomRenderer);

        void setPlanarReflectionRenderer(std::unique_ptr<Renderer::PlanarReflectionRenderer> planarReflectionRenderer);

        void setWidth(int width);

        void setHeight(int height);

        void setProjection(const glm::mat4 &projection);

        void resize(int width, int height, const glm::mat4 &projection);

        void enableShadows();

        void disableShadows();

        void toggleShadows();

        void toggleBloom();

        void toggleFog();

        void toggleReflections();

        [[nodiscard]] bool isReflectionsEnabled() const;

        // Callback hook pro reflections toggle. Game layer (MainScene) drží
        // konkrétní PlanarReflectionFeature - když se globální state změní,
        // RenderManager invokne callback, ten zmutuje feature. Pattern stejný
        // jako fog (fogFeature v ResourceManager), ale reflection je per-scene
        // ne global resource, proto callback místo shared instance.
        using ReflectionsToggleCallback = std::function<void(bool enabled)>;
        void setReflectionsCallback(ReflectionsToggleCallback cb);

        [[nodiscard]] bool isFogEnabled() const;
        [[nodiscard]] bool isShadowsEnabled() const { return shadows; }
        [[nodiscard]] bool isBloomEnabled() const { return bloom; }

        [[nodiscard]] std::shared_ptr<Camera> getCamera() const { return camera; }
        [[nodiscard]] std::shared_ptr<ResourceManager> getResourceManager() const { return resourceManager; }

        void reloadShaders() const;

        void reset();

        [[nodiscard]] const std::vector<Renderer::RendererEntry> &getRenderers() const;

        void updateDirectionalLight(const std::shared_ptr<Lights::DirectionalLight> & light);

    protected:
        void updateShadows();

        std::vector<Renderer::RendererEntry> renderers;
        std::unique_ptr<Renderer::DepthMapRenderer> depthMapRenderer;
        std::unique_ptr<Renderer::BloomRenderer> bloomRenderer;
        std::unique_ptr<Renderer::PlanarReflectionRenderer> planarReflectionRenderer;
        std::shared_ptr<Lights::DirectionalLight> directionalLight;
        std::shared_ptr<ResourceManager> resourceManager;
        std::shared_ptr<Camera> camera;
        std::shared_ptr<Tools::ContextState> contextState;
        glm::mat4 projection{};
        int width;
        int height;
        bool shadows;
        bool bloom;
        bool reflections;
        bool fog;
        uint64_t gFrameId = 0;
        ReflectionsToggleCallback reflectionsCallback;
        FrameUbo frameUbo;
        FrameData frameData{};

        // Fills frameData from current camera/projection/lights state and
        // uploads+binds. PlanarReflectionRenderer can copy frameData,
        // overwrite view/viewPos for the mirror, and call uploadFrameUbo
        // with its own modified copy.
        void populateAndUploadFrameUbo();

    public:
        [[nodiscard]] const FrameData& getFrameData() const { return frameData; }
        FrameUbo& getFrameUbo() { return frameUbo; }
    };
} // Manager

#endif //SNAKE3_RENDERMANAGER_H
