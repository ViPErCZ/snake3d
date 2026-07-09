#ifndef SNAKE3_REFRACTIONRENDERER_H
#define SNAKE3_REFRACTIONRENDERER_H

#include <algorithm>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Renderer/Opengl/Scene/SceneRenderer.h>

namespace Manager {
    class RenderManager;
}

namespace Renderer {
    class RefractionRenderer final : public BaseRenderer {
    public:
        RefractionRenderer(const std::shared_ptr<Tools::ContextState> &contextState,
                           const std::shared_ptr<Manager::ResourceManager> &resManager,
                           const std::shared_ptr<Manager::Camera> &camera,
                           const glm::mat4 &projection,
                           int width, int height);
        ~RefractionRenderer() override;

        void updateRenderers(const std::vector<RendererEntry> &renderers);
        void render3D(float dt, uint64_t frameId) override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void renderShadowMap() override;
        void resize(int width, int height, const glm::mat4 &projection) override;

        void setRenderManager(Manager::RenderManager *rm) { renderManager = rm; }

        // Render the refraction at a fraction of the screen resolution (see the matching
        // method on PlanarReflectionRenderer). Default 1.0; a game opts in. Rebuilds the FBO.
        void setResolutionScale(float scale);

    protected:
        void initializeFramebuffer();
        void destroyFramebuffer();
        std::shared_ptr<Tools::ContextState> contextState;
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Manager::Camera> camera;
        glm::mat4 projection;
        std::vector<RendererEntry> renderers;

        unsigned int refractionFBO{};
        unsigned int refractionTexture{};
        unsigned int depthBuffer{};
        int width;
        int height;
        float fboScale = 1.0f;
        [[nodiscard]] int fboW() const { return std::max(1, static_cast<int>(static_cast<float>(width) * fboScale)); }
        [[nodiscard]] int fboH() const { return std::max(1, static_cast<int>(static_cast<float>(height) * fboScale)); }
        Manager::RenderManager *renderManager = nullptr;
    };
}

#endif //SNAKE3_REFRACTIONRENDERER_H
