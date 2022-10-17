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
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
            shader->use();
            shader->setMat4("projection", this->projection);
            shader->setInt("texture_diffuse1", 0);
            shader->setFloat("alpha", 1.0);

            glLoadIdentity();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, resourceManager->getTexture("red_screen.bmp"));

            glm::vec3 position = radar->getPosition();
            glm::vec3 zoom = radar->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, position);
            model = glm::scale(model, glm::vec3(zoom.x, zoom.y, zoom.z));

            shader->setMat4("model", model);

            auto modelIter = this->model->getRadarMesh().begin();

            (*modelIter).first->bind();
            glDrawElements(GL_TRIANGLES, (int) (*modelIter).second->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            glDisable(GL_BLEND);

            int index = 0;
            for (auto mesh: this->model->getItemsMeshes()) {
                glLoadIdentity();
                auto Iter = radar->getItems().begin() + index;

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, (*Iter).radarPresent->getPrimaryTexture());

                glm::vec3 position = (*Iter).radarPresent->getPosition();
                glm::vec3 zoom = (*Iter).radarPresent->getZoom();

                // Initialize matrices
                glm::mat4 model = glm::mat4(1.0f);

                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(zoom.x, zoom.y, zoom.z));

                shader->setMat4("model", model);

                glDrawElements(GL_TRIANGLES, (int) mesh.second->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                index++;
            }

            radar->updatePositions();
        }
    }

    void RadarRenderer::beforeRender() {
        glDepthMask(0);
    }

    void RadarRenderer::afterRender() {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
    }

} // Renderer