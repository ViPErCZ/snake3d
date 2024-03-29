#ifndef SNAKE3_DEPTHMAPRENDERER_H
#define SNAKE3_DEPTHMAPRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>

using namespace Manager;

namespace Renderer {

    const int SHADOW_WIDTH = 4096;
    const int SHADOW_HEIGHT = 4096;

    class DepthMapRenderer : public BaseRenderer {
    public:
        DepthMapRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void renderQuad();
        void renderShadowMap() override;

    protected:
        ResourceManager* resourceManager;
        ShaderManager* shader;
        Camera* camera;
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
