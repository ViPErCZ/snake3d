#ifndef SNAKE3_PLANARREFLECTIONRENDERER_H
#define SNAKE3_PLANARREFLECTIONRENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Renderer/Opengl/Scene/SceneRenderer.h>

namespace Manager {
    // Forward declared - RenderManager.h includes this header, so we can't
    // include it back. PlanarReflectionRenderer.cpp pulls the full type.
    class RenderManager;
}

namespace Renderer {
    class PlanarReflectionRenderer final : public BaseRenderer {
    public:
        PlanarReflectionRenderer(const std::shared_ptr<Tools::ContextState> &contextState,
                                 const std::shared_ptr<Manager::ResourceManager> &resManager,
                                 const std::shared_ptr<Manager::Camera> &camera,
                                 const glm::mat4 &projection,
                                 int width, int height);
        ~PlanarReflectionRenderer() override;

        void updateRenderers(const std::vector<RendererEntry> &renderers);
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
        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Manager::Camera> camera;
        glm::mat4 projection;
        std::vector<Scenes::RendererEntry3D> nodes3d;
        std::vector<RendererEntry> renderers;

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
