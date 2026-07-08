#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include <snake3d/Renderer/Opengl/BaseRenderer.h>
#include <snake3d/Renderer/Opengl/DepthMapRenderer.h>
#include <snake3d/Renderer/Opengl/BloomRenderer.h>
#include <snake3d/Renderer/Opengl/PlanarReflectionRenderer.h>
#include <snake3d/Renderer/Opengl/RefractionRenderer.h>
#include <snake3d/Manager/FrameUbo.h>
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

        // shadowResolution: cascade depth-texture side. Default 4096; a top-down RTS can
        // pass 2048 (focus-box covers a small ground area) to cut shadow fill 4x.
        void initShadowMapping(int shadowResolution = Renderer::SHADOW_WIDTH);

        void initReflection();
        // Set the planar-reflection mirror plane dot(normal,p)=offset (after initReflection).
        // Default is the legacy +Z plane; a Y-up world (RTS water) passes (0,1,0), height.
        void setReflectionPlane(const glm::vec3 &normal, float offset);

        // Refraction pass: renders the scene (minus water) from the real camera into
        // "RefractionTexture" so water shaders can sample the wavy refracted bottom. Runs
        // under the same enable flag as reflections (toggleReflections / F2).
        void initRefraction();

        // Render the planar-reflection + refraction passes at a FRACTION of the screen
        // resolution (e.g. 0.5 = a quarter of the fill each). A big fill saving on the two
        // extra full-scene passes; the consuming shader samples the result by normalized
        // screen UV, so a lower-res target only softens it slightly. Call after
        // initReflection/initRefraction. 1.0 = full resolution (the default).
        void setReflectionResolutionScale(float scale);

        // Mirrored-camera view-projection of the last reflection pass (identity until the
        // first pass). Water shaders project their world position with it to sample the
        // reflection texture - world-anchored (0 A.D.'s reflectionMatrix technique).
        [[nodiscard]] glm::mat4 getReflectionViewProj() const {
            return planarReflectionRenderer ? planarReflectionRenderer->getMirrorViewProj() : glm::mat4(1.0f);
        }

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

        // Opt-in front-face culling for the shadow depth pass (default off). Casters
        // that are closed solids (correct outward winding) get acne-free, crisp cast
        // shadows; useful for large scenes where bias alone can't separate self-shadow
        // from cast shadow. Existing examples leave it off -> behaviour unchanged.
        void setShadowFrontFaceCull(bool enabled) { shadowFrontCull = enabled; }

        // Opt-in stable focus-box shadow for top-down/RTS cameras (see
        // DepthMapRenderer::setFocusBoxShadow). Call after initShadowMapping().
        void setShadowFocusBox(bool enabled, float radius) {
            if (depthMapRenderer) depthMapRenderer->setFocusBoxShadow(enabled, radius);
        }

        // Slope-scaled depth-bias (glPolygonOffset factor=units) for the shadow pass.
        // Default 3.0 suits examples without front-face culling; with front-cull on
        // (RTS) a smaller value avoids peter-panning (the green gap between a caster
        // and its contact shadow) while front-cull still prevents acne.
        void setShadowDepthBias(float bias) { shadowDepthBias = bias; }

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
        // Swap the camera used to populate the per-frame FrameData UBO (view/
        // projection/viewPos). Needed because Application::setCamera() may run in
        // onInit() AFTER this manager was constructed with the default camera;
        // without propagating here the frame UBO keeps using the old camera and
        // the whole scene renders from the wrong viewpoint. Call this before
        // initShadowMapping()/initReflection() (those capture the camera at
        // construction); the default Application flow already does (setCamera in
        // onInit, shadow/reflection init later or not at all).
        void setCamera(const std::shared_ptr<Camera>& cam) { camera = cam; }
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
        std::unique_ptr<Renderer::RefractionRenderer> refractionRenderer;
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
        // Opt-in: front-face-cull the shadow depth pass to kill self-shadow acne on
        // closed casters (off by default so existing examples are unchanged).
        bool shadowFrontCull = false;
        float shadowDepthBias = 3.0f;
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
