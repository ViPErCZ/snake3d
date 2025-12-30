#include "BoltRenderer.h"
#include <random>

namespace Renderer {
    BoltRenderer::BoltRenderer(Camera *camera, glm::mat4 proj, ResourceManager *resManager) {
        this->camera = camera;
        this->projection = proj;
        this->resourceManager = resManager;
        this->shader = resourceManager->getShader("boltShader").get();
        this->timeSinceLastBolt = 0.0f;
        this->nextBoltTime = 3.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f; // 5-15 sekund
        this->isActive = false;
        this->lightning = new LightningFlashEffect(camera, proj, resManager);
        this->lightning->init();

        createBoltGeometry();
    }

    BoltRenderer::~BoltRenderer() {
        cleanup();
    }

    void BoltRenderer::cleanup() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        delete lightning;
    }


    void BoltRenderer::generateBoltSegments(const glm::vec3 &start, const glm::vec3 &end) {
        segments.clear();

        const int numSegments = 25;
        const float maxOffset = 1.5f;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> displace(-maxOffset, maxOffset);
        std::uniform_real_distribution<float> brightness(0.8f, 1.2f);

        glm::vec3 direction = end - start;
        glm::vec3 segmentStep = direction / static_cast<float>(numSegments);
        glm::vec3 current = start;

        // Vytvoříme základní kolmý vektor pro celý blesk
        glm::vec3 mainPerpendicular = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));

        // Hlavní dráha blesku
        for (int i = 0; i < numSegments; i++) {
            BoltSegment segment;
            segment.start = current;

            // Přidáme náhodné vychýlení pro další bod
            glm::vec3 next = current + segmentStep;
            if (i != numSegments - 1) {
                // Poslední segment necháme mířit přímo do cíle
                next += mainPerpendicular * displace(gen);
                next += glm::vec3(0, 1, 0) * displace(gen) * 0.5f;
            }

            segment.end = next;
            segment.thickness = 0.15f * (1.0f - static_cast<float>(i) / numSegments * 0.5f);
            segment.brightness = brightness(gen);

            segments.push_back(segment);

            // Občas přidáme menší odbočku
            if (i > 0 && i < numSegments - 2) {
                if (static_cast<float>(rand()) / RAND_MAX < 0.2f) {
                    // 20% šance na odbočku
                    BoltSegment branch;
                    branch.start = current;
                    glm::vec3 branchEnd = current + segmentStep * 0.5f;
                    branchEnd += mainPerpendicular * displace(gen) * 2.0f;
                    branch.end = branchEnd;
                    branch.thickness = segment.thickness * 0.5f;
                    branch.brightness = segment.brightness * 0.7f;
                    segments.push_back(branch);
                }
            }

            current = next;
        }

        // Zajistíme, že poslední segment končí přesně v cílovém bodě
        if (!segments.empty()) {
            segments.back().end = end;
        }
    }

    void BoltRenderer::beforeRender(const MODE mode) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        this->mode = mode;
    }

    void BoltRenderer::afterRender() {
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void BoltRenderer::render3D(float dt, uint64_t frameId) {
        if (!isActive) return;

        timeSinceLastBolt += dt;

        shader->use();
        shader->setMat4("projection", projection);
        shader->setMat4("view", camera->getViewMatrix());
        shader->setFloat("time", static_cast<float>(glfwGetTime()));

        glLineWidth(3.0f);

        for (const auto& segment : segments) {
            shader->setVec3("startPos", segment.start);
            shader->setVec3("endPos", segment.end);
            shader->setFloat("brightness", segment.brightness * 2.0f);

            glBindVertexArray(VAO);
            glDrawArrays(GL_LINES, 0, 2);
        }

        glLineWidth(1.0f);

        if (timeSinceLastBolt > 0.15f) {
            isActive = false;
        }

        lightning->update(dt);
        lightning->render();
    }

    void BoltRenderer::triggerBolt() {
        isActive = true;

        // Získáme pozici a směr kamery
        glm::vec3 camPos = camera->getPosition();
        glm::vec3 camFront = camera->getFront();
        glm::vec3 camRight = glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f)));

        // Náhodné hodnoty pro pozicování
        float forwardOffset = 10.0f + static_cast<float>(rand()) / RAND_MAX * 15.0f;
        float sideOffset = -8.0f + static_cast<float>(rand()) / RAND_MAX * 16.0f;
        float heightOffset = 15.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f;

        // Počáteční pozice blesku
        glm::vec3 startPos = camPos +
                             camFront * forwardOffset + // Vzdálenost před kamerou
                             camRight * sideOffset + // Posun do stran
                             glm::vec3(0.0f, heightOffset, 0.0f); // Výška

        // Koncový bod - vždy směřuje k zemi, ale s mírnou náhodnou odchylkou
        glm::vec3 endPos = startPos + glm::vec3(
                               -2.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f, // Náhodná X odchylka
                               -heightOffset - 5.0f, // Dolů k zemi
                               -2.0f + static_cast<float>(rand()) / RAND_MAX * 4.0f // Náhodná Z odchylka
                           );

        generateBoltSegments(startPos, endPos);

        // Reset časovače
        timeSinceLastBolt = 0.0f;

        lightning->triggerSequence({1.0f, 0.6f, 0.8f}, 0.05f);
    }

    void BoltRenderer::renderShadowMap() {
    }

    void BoltRenderer::createBoltGeometry() {
        constexpr float vertices[] = {
            0.0f, 0.0f, 0.0f,  // První bod
            1.0f, 0.0f, 0.0f   // Druhý bod
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}
