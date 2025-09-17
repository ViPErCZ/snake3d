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

    class DepthMapRenderer : public BaseRenderer {
    public:
        DepthMapRenderer(Camera *camera, glm::mat4 proj, ResourceManager *resManager);

        void render(float dt) override;

        void beforeRender() override;

        void afterRender() override;

        void renderQuad();

        void renderShadowMap() override;

        void computeLightSpaceMatrix(shared_ptr<DirectionalLight> &light, glm::vec3 lightTarget, glm::vec3 sceneMin,
                                     glm::vec3 sceneMax);

    protected:
        shared_ptr<Mesh> getMesh() override;

        ResourceManager *resourceManager;
        ShaderManager *shader;
        Camera *camera;
        glm::mat4 projection{};
        unsigned int depthMapFBO{};
        unsigned int depthMap{};
        glm::mat4 lightSpaceMatrix{};
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        glm::vec3 lightPos{};
    };
} // Renderer

#endif //SNAKE3_DEPTHMAPRENDERER_H
