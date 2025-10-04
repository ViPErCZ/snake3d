#ifndef SNAKE3_DEPTHMAPRENDERER_H
#define SNAKE3_DEPTHMAPRENDERER_H

#include "BaseRenderer.h"
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

        void render(float dt) const;

        void beforeRender(int index) const;

        void afterRender();

        void renderQuad();

        void renderShadowMap();

        void bind(int index, const glm::mat4 &lightSpaceMatrix) const;

        std::vector<glm::mat4> computeLightSpaceMatrix(
            shared_ptr<DirectionalLight> &light, glm::vec3 lightTarget,
            glm::vec3 sceneMin,
            glm::vec3 sceneMax
        );

        std::vector<glm::mat4> computeLightSpaceMatrixForPlane(
            shared_ptr<DirectionalLight> &light,
            const glm::vec3 &planeCenter,
            float planeWidth,
            float planeHeight
        );

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
        glm::vec3 lightPos{};
        float cascadeSplits[NUM_CASCADES] = {0.05f, 0.2f, 1.0f};

        [[nodiscard]] std::vector<glm::vec3> getFrustumCornersWorldSpace(
            const glm::mat4 &proj, const glm::mat4 &view,
            float nearPlane,
            float farPlane
        ) const;
    };
} // Renderer

#endif //SNAKE3_DEPTHMAPRENDERER_H
