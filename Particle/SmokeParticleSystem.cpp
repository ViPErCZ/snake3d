#include "SmokeParticleSystem.h"

#include <glm/gtc/random.hpp>

namespace Particle {
    // Vytvoříme si pomocnou strukturu pro posílání dat do VBO, aby odpovídala atributům v shaderu
    struct SmokeParticleGPUData {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 sizeAndRotation;
    };

    SmokeParticleSystem::SmokeParticleSystem(ResourceManager& resourceManager, int maxParticles)
        : resourceManager(resourceManager), maxParticles(maxParticles) {
        init();
    }

    void SmokeParticleSystem::init() {
        particles.resize(maxParticles);
        // Vytvoření VAO a VBO je identické jako u ohně
        float quadVertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 1.0f
        };
        unsigned int indices[] = {0, 1, 2, 2, 3, 0};
        unsigned int EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &quadVBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));


        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(SmokeParticleGPUData), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(2); // Pozice
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SmokeParticleGPUData), (void*)offsetof(SmokeParticleGPUData, position));
        glEnableVertexAttribArray(3); // Barva
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SmokeParticleGPUData), (void*)offsetof(SmokeParticleGPUData, color));
        glEnableVertexAttribArray(4); // Velikost a rotace
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(SmokeParticleGPUData), (void*)offsetof(SmokeParticleGPUData, sizeAndRotation));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);

        glBindVertexArray(0);
    }

    void SmokeParticleSystem::update(float dt, glm::vec3 offset) {
        // Přidáme méně kouřových částic, ale s offsetem
        addParticle(offset);

        for (int i = 0; i < maxParticles; ++i) {
            SmokeParticle& p = particles[i];
            if (p.life > 0.0f) {
                p.life -= dt;
                p.position += p.velocity * dt;

                // Přidáme lehký vítr, který kouř odnáší
                p.position.x += 0.1f * dt;

                p.rotation += p.rotationSpeed * dt;
                p.size += dt * 0.05f; // Zpomalíme růst velikosti
                p.color.a = glm::smoothstep(0.0f, 0.5f, p.life / 5.0f); // Bledne pomaleji a více postupně
            }
        }
    }

    void SmokeParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        // Použijeme nový shader pro kouř
        const auto smokeShader = this->resourceManager.getShader("smoke");
        smokeShader->use();
        smokeShader->setMat4("view", view);
        smokeShader->setMat4("projection", projection);
        this->resourceManager.getTexture("smoke.png")->bind();

        // Připravíme data pro GPU
        std::vector<SmokeParticleGPUData> gpuData;
        gpuData.reserve(particles.size());
        for (const auto& p : particles) {
            if (p.life > 0.0f) {
                gpuData.push_back({p.position, p.color, {p.size, p.rotation}});
            }
        }

        if (!gpuData.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, gpuData.size() * sizeof(SmokeParticleGPUData), gpuData.data());
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, gpuData.size());
            glBindVertexArray(0);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void SmokeParticleSystem::addParticle(glm::vec3 offset) {
        for (int i = 0; i < maxParticles; ++i) {
            int index = (lastUsedParticle + i) % maxParticles;
            if (particles[index].life <= 0.0f) {
                SmokeParticle& p = particles[index];

                // Rodí se v úzkém kruhu a na pozici ohně
                glm::vec2 spawnDisk = glm::diskRand(0.1f); // Menší poloměr
                p.position = glm::vec3(spawnDisk.x, 0.0f, spawnDisk.y) + offset;

                // Rychlost je hlavně vzhůru, s minimálním pohybem do stran
                p.velocity.x = glm::linearRand(-0.05f, 0.05f);
                p.velocity.y = glm::linearRand(0.3f, 0.7f); // Pomalejší stoupání
                p.velocity.z = glm::linearRand(-0.05f, 0.05f);

                float greyTone = glm::linearRand(0.1f, 0.25f);
                p.color = glm::vec4(greyTone + 0.02f, greyTone, greyTone, 0.5f);
                p.life = 5.0f;

                p.size = glm::linearRand(0.05f, 0.1f); // Začíná menší
                p.rotation = glm::linearRand(0.0f, 2.0f * 3.14159f);
                p.rotationSpeed = glm::linearRand(-0.2f, 0.2f);

                lastUsedParticle = index;
                return;
            }
        }
    }
} // Particle
