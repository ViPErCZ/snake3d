#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../Renderer/Opengl/BloomRenderer.h"
#include "../Renderer/Opengl/PlanarReflectionRenderer.h"
#include <vector>

using namespace std;
using namespace Renderer;

namespace Manager {
    enum class RenderPassType {
        Shadow,
        Scene,
        PostProcess
    };

    class RenderManager final {
    public:
        RenderManager(const shared_ptr<ContextState> &contextState,
                      const shared_ptr<Camera> &camera, const shared_ptr<ResourceManager> &resourceManager,
                      const glm::mat4 &projection,
                      int width, int height);

        ~RenderManager() = default;

        [[nodiscard]] shared_ptr<ContextState> getContextState() const;

        void initBloom();

        void initShadowMapping();

        void initReflection();

        void render(float dt);

        void addRenderer(shared_ptr<BaseRenderer> renderer, int priority = 0);
        bool removeRenderer(const shared_ptr<BaseRenderer> &renderer);

        void setDepthMapRenderer(unique_ptr<DepthMapRenderer> &depthMapRenderer);

        void setBloomRenderer(unique_ptr<BloomRenderer> &bloomRenderer);

        void setPlanarReflectionRenderer(unique_ptr<PlanarReflectionRenderer> planarReflectionRenderer);

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

        [[nodiscard]] bool isFogEnabled() const;

        void reset();

        [[nodiscard]] const vector<RendererEntry> &getRenderers() const;

        void updateDirectionalLight(const shared_ptr<DirectionalLight> & light);

    protected:
        void updateShadows();

        vector<RendererEntry> renderers;
        unique_ptr<DepthMapRenderer> depthMapRenderer;
        unique_ptr<BloomRenderer> bloomRenderer;
        unique_ptr<PlanarReflectionRenderer> planarReflectionRenderer;
        shared_ptr<DirectionalLight> directionalLight;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<Camera> camera;
        shared_ptr<ContextState> contextState;
        glm::mat4 projection{};
        int width;
        int height;
        bool shadows;
        bool bloom;
        bool reflections;
        bool fog;
        uint64_t gFrameId = 0;
    };
} // Manager

#endif //SNAKE3_RENDERMANAGER_H
