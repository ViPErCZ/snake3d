#ifndef SNAKE3_BLOOMRENDERER_H
#define SNAKE3_BLOOMRENDERER_H

#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderProgram.h"
#include "BaseRenderer.h"

using namespace Manager;
using namespace std;

namespace Renderer {
    class BloomRenderer final : public BaseRenderer {
    public:
        explicit BloomRenderer(const shared_ptr<ResourceManager> &resManager, int width, int height);
        ~BloomRenderer() override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void render3D(float dt, uint64_t frameId) override;
        void renderShadowMap() override;
        void resize(int width, int height, const glm::mat4 &projection) override;
    protected:
        void renderQuad();
        void initializeFramebuffers();
        void destroyFramebuffers();
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ShaderProgram> shader;
        shared_ptr<ShaderProgram> shaderBlur;
        shared_ptr<ShaderProgram> shaderBloomFinal;
        unsigned int hdrFBO{};
        unsigned int pingpongFBO[2]{};
        unsigned int colorBuffers[2]{};
        unsigned int pingpongColorBuffers[2]{};
        unsigned int rboDepth{};
        unsigned int quadVAO = 0;
        unsigned int quadVBO{};
        int width;
        int height;
    };
}

#endif //SNAKE3_BLOOMRENDERER_H
