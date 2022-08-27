#include "RadarRenderer.h"

namespace Renderer {
    RadarRenderer::RadarRenderer(Radar *radar, ShaderManager *shader, Camera *camera, glm::mat4 proj,
                                 ResourceManager *resManager) : radar(radar) {
        resourceManager = resManager;
        this->shader = shader;
        this->camera = camera;
        this->projection = proj;
        model = new RadarModel(radar);
    }

    RadarRenderer::~RadarRenderer() {
        delete model;
        delete radar;
    }

    void RadarRenderer::render() {
        if (radar->isVisible()) {
            shader->use();
            shader->setMat4("projection", this->projection);
            shader->setInt("texture_diffuse1", 0);
            shader->setFloat("alpha", 1.0);

            for (auto Iter = radar->getItems().begin(); Iter < radar->getItems().end(); Iter++) {
                glLoadIdentity();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("red_screen.bmp"));

                glm::vec3 position = (*Iter).item->getPosition();

                // Initialize matrices
                glm::mat4 model = glm::mat4(1.0f);

                position.x = 125;
                position.y = 160;
                position.z = -1;
                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(100, 100, 1));

                shader->setMat4("model", model);

                auto modelIter = this->model->getMeshes().begin();

                (*modelIter).first->bind();
                glDrawElements(GL_TRIANGLES, (int) (*modelIter).second->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                break;
            }

            radar->updatePositions();
        }
    }

    void RadarRenderer::beforeRender() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
        glDepthMask(0);
    }

    void RadarRenderer::afterRender() {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
    }

} // Renderer