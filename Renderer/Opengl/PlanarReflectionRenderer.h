#ifndef SNAKE3_PLANARREFLECTIONRENDERER_H
#define SNAKE3_PLANARREFLECTIONRENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "BaseRenderer.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "Scene/SceneRenderer.h"

using namespace std;
using namespace Manager;

namespace Manager {
    // Forward declared - RenderManager.h includes this header, so we can't
    // include it back. PlanarReflectionRenderer.cpp pulls the full type.
    class RenderManager;
}

namespace Renderer {
    class PlanarReflectionRenderer final : public BaseRenderer {
    public:
        PlanarReflectionRenderer(const shared_ptr<ContextState> &contextState,
                                 const shared_ptr<ResourceManager> &resManager,
                                 const shared_ptr<Camera> &camera, 
                                 const glm::mat4 &projection,
                                 int width, int height);
        ~PlanarReflectionRenderer() override;

        void updateRenderers(const vector<RendererEntry> &renderers);
        void render3D(float dt, uint64_t frameId) override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void renderShadowMap() override;
        void setPlaneZ(float z);
        void resize(int width, int height, const glm::mat4 &projection) override;

        // Non-owning - set by RenderManager so reflection pass can read
        // current FrameData (with dirLight + lights), copy it, swap the
        // mirrored view/viewPos, and upload via the shared FrameUbo.
        void setRenderManager(Manager::RenderManager* rm) { renderManager = rm; }

    protected:
        void initializeFramebuffer();
        void destroyFramebuffer();
        shared_ptr<ContextState> contextState;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        vector<Scenes::RendererEntry3D> nodes3d;
        vector<RendererEntry> renderers;

        unsigned int reflectionFBO{};
        unsigned int reflectionTexture{};
        unsigned int depthBuffer{};
        int width;
        int height;
        float planeZ = -1.0f;
        Manager::RenderManager* renderManager = nullptr;
    };
}

#endif //SNAKE3_PLANARREFLECTIONRENDERER_H
