#include "LightningFlashEffect.h"
#include <GLFW/glfw3.h>

namespace Effects {
    LightningFlashEffect::LightningFlashEffect(Camera* camera, glm::mat4 proj, ResourceManager* resManager) {
        this->camera = camera;
        this->projection = proj;
        this->resourceManager = resManager;
        this->flashShader = resourceManager->getShader("flash");
    }

    void LightningFlashEffect::init() {
        setupFullscreenQuad();
        flashShader->use();
        flashShader->setInt("screenTexture", 0);
    }

    void LightningFlashEffect::trigger() {
        flashing = true;
        timer = 0.0f;
        alpha = 1.0f;
    }

    void LightningFlashEffect::update(float deltaTime) {
        if (!flashing) return;

        timer += deltaTime;
        alpha = 1.0f - (timer / duration);

        if (timer >= duration) {
            alpha = 0.0f;
            flashing = false;
        }
    }

    bool LightningFlashEffect::isActive() const {
        return flashing || alpha > 0.0f;
    }

    void LightningFlashEffect::render() const {
        if (alpha <= 0.0f) return;

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        flashShader->use();
        flashShader->setFloat("alpha", alpha);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    void LightningFlashEffect::setupFullscreenQuad() {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }
} // Effects