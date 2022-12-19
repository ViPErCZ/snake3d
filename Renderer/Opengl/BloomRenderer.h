#ifndef SNAKE3_BLOOMRENDERER_H
#define SNAKE3_BLOOMRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <vector>

using namespace Manager;
using namespace std;

namespace Renderer {
    class BloomRenderer : public BaseRenderer {
    public:
        explicit BloomRenderer(ResourceManager* resManager, int width, int height);
        void beforeRender() override;
        void afterRender() override;
        void render() override;
        void renderShadowMap() override;
    protected:
        void renderQuad();
        ResourceManager* resourceManager;
        ShaderManager* shader;
        ShaderManager* shaderBlur;
        ShaderManager* shaderBloomFinal;
        unsigned int hdrFBO{};
        unsigned int pingpongFBO[2]{};
        unsigned int colorBuffers[2]{};
        unsigned int pingpongColorbuffers[2]{};
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        int width;
        int height;
    };
}

#endif //SNAKE3_BLOOMRENDERER_H
