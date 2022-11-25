#ifndef SNAKE3_DEPTHMAPRENDERER_H
#define SNAKE3_DEPTHMAPRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>

using namespace Manager;

namespace Renderer {

    class DepthMapRenderer : public BaseRenderer {
    public:
        DepthMapRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void renderScene(const ShaderManager *shader);
        void renderQuad();
        void renderCube();

    protected:
        ResourceManager* resourceManager;
        ShaderManager* shader;
        Camera* camera;
        glm::mat4 projection{};
        unsigned int depthMapFBO{};
        unsigned int depthMap{};
        glm::mat4 lightSpaceMatrix{};
        int width;
        int height;
        float test;
        unsigned int planeVAO;
        unsigned int planeVBO;
        unsigned int quadVAO = 0;
        unsigned int quadVBO;
        unsigned int cubeVAO = 0;
        unsigned int cubeVBO = 0;
        glm::vec3 lightPos;
    };

} // Renderer

#endif //SNAKE3_DEPTHMAPRENDERER_H
