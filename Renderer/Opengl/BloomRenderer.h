#ifndef SNAKE3_BLOOMRENDERER_H
#define SNAKE3_BLOOMRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <vector>

using namespace Manager;
using namespace std;

namespace Renderer {
    class BloomRenderer : public BaseRenderer {
    public:
        BloomRenderer(Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        void afterRender() override;
        void render() override;

    protected:
        void renderQuad();
        ResourceManager* resourceManager;
        ShaderManager* shader;
        ShaderManager* shaderBlur;
        ShaderManager* shaderBloomFinal;
        Camera* camera;
        glm::mat4 projection{};
        vector<glm::vec3> lightPositions;
        vector<glm::vec3> lightColors;
        unsigned int hdrFBO{};
        unsigned int pingpongFBO[2]{};
        unsigned int colorBuffers[2]{};
        unsigned int pingpongColorbuffers[2]{};
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
    public:
        void renderShadowMap() override;

        void beforeRender() override;
    };
}

#endif //SNAKE3_BLOOMRENDERER_H
