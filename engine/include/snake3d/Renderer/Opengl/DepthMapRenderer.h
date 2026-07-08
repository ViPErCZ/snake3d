#ifndef SNAKE3_DEPTHMAPRENDERER_H
#define SNAKE3_DEPTHMAPRENDERER_H

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Manager/Camera.h>
#include <snake3d/Lights/DirectionalLight.h>

namespace Renderer {
    constexpr int SHADOW_WIDTH = 4096;
    constexpr int SHADOW_HEIGHT = 4096;
    constexpr int NUM_CASCADES = 3;

    class DepthMapRenderer  {
    public:
        // shadowResolution: side of the square cascade depth texture. Default SHADOW_WIDTH
        // (4096). A top-down RTS focus-box covers a small ground area, so a lower value
        // (e.g. 2048) is plenty and cuts the shadow-pass fill 4x; pass it per game.
        DepthMapRenderer(Manager::Camera *camera, const glm::mat4 &proj, Manager::ResourceManager *resManager,
                         int shadowResolution = SHADOW_WIDTH);

        ~DepthMapRenderer() = default;

        void render(float dt) const;

        void beforeRender(int index) const;

        void afterRender() = delete;

        void renderShadowMap();

        void bind(int index, const glm::mat4 &lightSpaceMatrix) const;

        std::vector<glm::mat4> computeLightSpaceMatrix(const std::shared_ptr<Lights::DirectionalLight> &light,
                                                      glm::vec3 sceneMin, glm::vec3 sceneMax);

        // Opt-in stable shadow box for top-down (RTS-style) cameras. Instead of fitting
        // the cascades to the camera frustum - which for a steep top-down camera extends
        // far below the ground, blowing up the light-space depth range and wrecking
        // precision (shadows stretch/smear, especially on tall objects, and shift while
        // panning) - this fits ONE tight ortho box of half-extent `baseRadius * (camY *
        // heightScale)` centred on the camera's ground focus point. Stable on pan,
        // high precision. Default off -> frustum-fit cascades for other examples.
        void setFocusBoxShadow(bool enabled, float radius) { focusBoxMode = enabled; focusBoxRadius = radius; }

    protected:
        Manager::ResourceManager *resourceManager;
        Manager::ShaderProgram *shader;
        Manager::Camera *camera;
        glm::mat4 projection{};
        GLuint depthMapFBO{};
        GLuint depthMap{};
        std::vector<glm::mat4> lightSpaceMatrices;
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        float cascadeSplits[NUM_CASCADES] = {0.1f, 0.3f, 1.0f};
        float cascadeEndsWorld[NUM_CASCADES] = {};
        bool focusBoxMode = false;   // RTS-style stable shadow box (see setFocusBoxShadow)
        float focusBoxRadius = 60.0f; // world half-extent of the box on the ground
        int shadowRes = SHADOW_WIDTH; // square cascade depth-texture side (ctor arg)

        [[nodiscard]] std::vector<glm::vec3> getFrustumCornersWorldSpace(
            float nearPlane,
            float farPlane
        ) const;
    };
} // Renderer

#endif //SNAKE3_DEPTHMAPRENDERER_H
