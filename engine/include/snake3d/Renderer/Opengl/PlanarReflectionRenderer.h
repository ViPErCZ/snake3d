#ifndef SNAKE3_PLANARREFLECTIONRENDERER_H
#define SNAKE3_PLANARREFLECTIONRENDERER_H

#include <algorithm>
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
        // Generalized mirror plane: reflect across plane dot(normal, p) = offset. The
        // default (normal = +Z, offset = planeZ) reproduces the legacy Z-plane behavior,
        // so snake3 is unchanged. A Y-up world (e.g. RTS water on the XZ ground) sets
        // normal = (0,1,0), offset = water height. normal is normalized internally.
        void setReflectionPlane(const glm::vec3 &normal, float offset);
        void resize(int width, int height, const glm::mat4 &projection) override;

        // Render the reflection at a FRACTION of the screen resolution (e.g. 0.5 =
        // half on each axis = a quarter of the fill). The consuming shader samples the
        // texture by normalized screen UV, so a lower-res target is transparent to it
        // (just a softer reflection - barely visible on a wavy surface). Default 1.0
        // (full res) so existing examples are unchanged; a game opts in. Rebuilds the
        // FBO at the new size.
        void setResolutionScale(float scale);

        // Non-owning - set by RenderManager so reflection pass can read
        // current FrameData (with dirLight + lights), copy it, swap the
        // mirrored view/viewPos, and upload via the shared FrameUbo.
        void setRenderManager(Manager::RenderManager* rm) { renderManager = rm; }

        // The mirrored camera's view-projection from the last reflection pass. Water
        // shaders project their fragment's WORLD position with it to sample the
        // reflection texture (0 A.D.'s reflectionMatrix technique) - world-anchored,
        // so the reflection doesn't swim with the main camera.
        [[nodiscard]] const glm::mat4 &getMirrorViewProj() const { return mirrorViewProj; }

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
        float fboScale = 1.0f; // reflection target resolution fraction (see setResolutionScale)
        [[nodiscard]] int fboW() const { return std::max(1, static_cast<int>(static_cast<float>(width) * fboScale)); }
        [[nodiscard]] int fboH() const { return std::max(1, static_cast<int>(static_cast<float>(height) * fboScale)); }
        float planeZ = -1.0f;
        // Mirror plane dot(reflectNormal, p) = reflectOffset. Defaults to the legacy
        // +Z plane at planeZ; setReflectionPlane / setPlaneZ update both.
        glm::vec3 reflectNormal{0.0f, 0.0f, 1.0f};
        float reflectOffset = -1.0f;
        Manager::RenderManager* renderManager = nullptr;
        glm::mat4 mirrorViewProj{1.0f}; // mirrored camera viewProj of the last pass
    };
}

#endif //SNAKE3_PLANARREFLECTIONRENDERER_H
