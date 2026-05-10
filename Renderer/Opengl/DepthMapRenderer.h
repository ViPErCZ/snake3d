#ifndef SNAKE3_DEPTHMAPRENDERER_H
#define SNAKE3_DEPTHMAPRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "../../Lights/DirectionalLight.h"

using namespace Manager;
using namespace Lights;

namespace Renderer {
    constexpr int SHADOW_WIDTH = 4096;
    constexpr int SHADOW_HEIGHT = 4096;
    constexpr int NUM_CASCADES = 3;

    class DepthMapRenderer  {
    public:
        DepthMapRenderer(Camera *camera, const glm::mat4 &proj, ResourceManager *resManager);

        ~DepthMapRenderer() = default;

        void render(float dt) const;

        void beforeRender(int index) const;

        void afterRender() = delete;

        void renderShadowMap();

        void bind(int index, const glm::mat4 &lightSpaceMatrix) const;

        std::vector<glm::mat4> computeLightSpaceMatrix(const shared_ptr<DirectionalLight> &light,
                                                      glm::vec3 sceneMin, glm::vec3 sceneMax);

    protected:
        ResourceManager *resourceManager;
        ShaderManager *shader;
        Camera *camera;
        glm::mat4 projection{};
        GLuint depthMapFBO{};
        GLuint depthMap{};
        std::vector<glm::mat4> lightSpaceMatrices;
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        float cascadeSplits[NUM_CASCADES] = {0.1f, 0.3f, 1.0f};
        float cascadeEndsWorld[NUM_CASCADES] = {};

        [[nodiscard]] std::vector<glm::vec3> getFrustumCornersWorldSpace(
            float nearPlane,
            float farPlane
        ) const;
    };
} // Renderer

#endif //SNAKE3_DEPTHMAPRENDERER_H
