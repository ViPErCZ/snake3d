#ifndef SNAKE3_BLOOMRENDERER_H
#define SNAKE3_BLOOMRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"

using namespace Manager;
using namespace std;

namespace Renderer {
    class BloomRenderer final : public BaseRenderer {
    public:
        explicit BloomRenderer(const shared_ptr<ResourceManager> &resManager, int width, int height);
        void beforeRender() override;
        void afterRender() override;
        void render3D(float dt) override;
        void renderShadowMap() override;
    protected:
        void renderQuad();
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ShaderManager> shader;
        shared_ptr<ShaderManager> shaderBlur;
        shared_ptr<ShaderManager> shaderBloomFinal;
        unsigned int hdrFBO{};
        unsigned int pingpongFBO[2]{};
        unsigned int colorBuffers[2]{};
        unsigned int pingpongColorBuffers[2]{};
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        int width;
        int height;
    };
}

#endif //SNAKE3_BLOOMRENDERER_H
