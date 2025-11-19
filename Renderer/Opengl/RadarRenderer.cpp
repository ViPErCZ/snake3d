#include "RadarRenderer.h"

namespace Renderer {
    RadarRenderer::RadarRenderer(shared_ptr<Radar> &radar, const shared_ptr<Camera> &camera,
                                 const shared_ptr<ResourceManager> &resManager,
                                 const glm::mat4 &proj) : radar(radar), resourceManager(resManager), camera(camera),
                                                          projection(proj) {
        model = make_unique<RadarModel>(radar);
        shader = resourceManager->getShader("basic2d");
        frameTexture = resourceManager->getTexture("red_screen.bmp");
    }

    void RadarRenderer::render3D(float dt) {
        if (radar->isVisible()) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
            shader->use();
            shader->setMat4("projection", projection);
            shader->setInt("textureMap", 0);
            shader->setFloat("alpha", 1.0);

            frameTexture->bind();
            shader->setMat4("model", radar->getModelMatrix());
            shader->setBool("useMaterial", false);

            const auto radarMesh = this->model->getMesh();

            radarMesh->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(radarMesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
            glDisable(GL_BLEND);

            int index = 0;
            radar->updatePositions();

            for (const auto &radarItem: radar->getItems()) {
                if (radarItem.item->isVisible()) {

                    shader->setBool("useMaterial", true);
                    shader->setVec3("color", radarItem.color);
                    shader->setMat4("model", radarItem.radarPresent->getModelMatrix());

                    glDrawElements(GL_TRIANGLES, static_cast<int>(radarMesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
                    index++;
                }
            }

            frameTexture->unbind();
        }
    }

    void RadarRenderer::beforeRender() {
        glDepthFunc(GL_LEQUAL);
    }

    void RadarRenderer::afterRender() {
        glDepthFunc(GL_LESS);
    }
} // Renderer
