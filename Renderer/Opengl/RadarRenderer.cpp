#include "RadarRenderer.h"

namespace Renderer {
    RadarRenderer::RadarRenderer(Radar *radar, Camera *camera, glm::mat4 proj,
                                 ResourceManager *resManager) : radar(radar), camera(camera) {
        resourceManager = resManager;
        projection = proj;
        model = new RadarModel(radar);
        shader = resourceManager->getShader("radarShader");
        frameTexture = resourceManager->getTexture("red_screen.bmp");
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
            shader->setMat4("projection", projection);
            shader->setInt("texture_diffuse1", 0);
            shader->setFloat("alpha", 1.0);

            glLoadIdentity();

            frameTexture->bind();

            glm::vec3 position = radar->getPosition();
            glm::vec3 zoom = radar->getZoom();

            // Initialize matrices
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, position);
            model = glm::scale(model, glm::vec3(zoom.x, zoom.y, zoom.z));

            shader->setMat4("model", model);
            shader->setBool("useMaterial", false);

            auto radarMesh = this->model->getMesh();

            radarMesh->bind();
            glDrawElements(GL_TRIANGLES, (int) radarMesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
            glDisable(GL_BLEND);

            int index = 0;
            for (const auto& radarItem: radar->getItems()) {
                glLoadIdentity();

                if (radarItem.texture != nullptr) {
                    radarItem.texture->bind();
                    shader->setBool("useMaterial", false);
                } else {
                    shader->setBool("useMaterial", true);
                    shader->setVec3("Color", radarItem.color);
                }

                glm::vec3 position = radarItem.radarPresent->getPosition();
                glm::vec3 zoom = radarItem.radarPresent->getZoom();

                // Initialize matrices
                glm::mat4 model = glm::mat4(1.0f);

                model = glm::translate(model, position);
                model = glm::scale(model, glm::vec3(zoom.x, zoom.y, zoom.z));

                shader->setMat4("model", model);

                glDrawElements(GL_TRIANGLES, (int) radarMesh->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                index++;
            }

            radar->updatePositions();
        }
    }

    void RadarRenderer::beforeRender() {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    }

    void RadarRenderer::afterRender() {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(1);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

} // Renderer