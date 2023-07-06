#include "RainDropRenderer.h"
#include "Model/MeshModel.h"

namespace Renderer {
    RainDropRenderer::RainDropRenderer(BaseItem *item, Camera *camera, glm::mat4 proj,
                                       ResourceManager *resourceManager)
            : BaseRenderer(item), resourceManager(resourceManager), camera(camera), projection(proj), enable(false) {
        baseShader = resourceManager->getShader("rainDrop");
        texture = resourceManager->getTexture("raindrops_nor.png");
        model = new MeshModel(item);
    }

    RainDropRenderer::~RainDropRenderer() {
        delete model;
    }

    void RainDropRenderer::render() {
        if (enable) {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setBool("fogEnable", false);
            baseShader->setInt("texture_diffuse1", 0);
            baseShader->setBool("useBones", false);
            baseShader->setFloat("TIME", lastFrame);
            texture->bind(0);
            model->getMesh()->bind();

            float x = -0.5f;
            float y = -0.5f;

            for (int a = 0; a < 4; a++) {
                for (int b = 0; b < 4; b++) {
                    glm::mat4 tr = glm::mat4(1.0f);
                    // Transform the matrices to their correct form
                    tr = glm::translate(tr, {x, y, 0.0001});
                    tr = glm::scale(tr, {0.5, 0.5, 1.0});
                    baseShader->setMat4("model", tr);

                    glDrawElements(GL_TRIANGLES, (int) model->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                    y += 1.0f;
                }
                x += 1.0f;
                y = -0.5f;
            }

            glBindVertexArray(0);
        }
    }

    void RainDropRenderer::renderShadowMap() {

    }

    void RainDropRenderer::beforeRender() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        auto currentFrame = (float)glfwGetTime();
        if (currentFrame > lastFrame) {
            lastFrame = currentFrame;
        }
    }

    void RainDropRenderer::afterRender() {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);
    }

    void RainDropRenderer::setEnable(bool enable) {
        RainDropRenderer::enable = enable;
    }
} // Renderer