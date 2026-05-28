#ifndef SNAKE3_BLOOMRENDERER_H
#define SNAKE3_BLOOMRENDERER_H

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/ShaderProgram.h>
#include "BaseRenderer.h"

namespace Renderer {
    class BloomRenderer final : public BaseRenderer {
    public:
        explicit BloomRenderer(const std::shared_ptr<Manager::ResourceManager> &resManager, int width, int height);
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
        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Manager::ShaderProgram> shader;
        std::shared_ptr<Manager::ShaderProgram> shaderBlur;
        std::shared_ptr<Manager::ShaderProgram> shaderBloomFinal;
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
