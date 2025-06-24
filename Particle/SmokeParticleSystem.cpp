#include "SmokeParticleSystem.h"

#include <glm/gtc/random.hpp>

namespace Particle {
    SmokeParticleSystem::SmokeParticleSystem(ResourceManager &resourceManager, const int maxParticles)
        : maxParticles(maxParticles), resourceManager(resourceManager) {
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
        glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(FireParticle), nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle),
                              (void *) offsetof(FireParticle, position));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (void *) offsetof(FireParticle, color));
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glBindVertexArray(0);
    }

    void SmokeParticleSystem::update(float dt) {
        // Přidáváme méně kouřových částic než ohnivých
        addParticle();

        for (int i = 0; i < maxParticles; ++i) {
            FireParticle &p = particles[i];
            if (p.life > 0.0f) {
                p.life -= dt;
                p.position += p.velocity * dt;

                // Kouř postupně bledne
                p.color.a = glm::smoothstep(0.0f, 1.0f, p.life / 4.0f);
            }
        }
    }

    void SmokeParticleSystem::render(const glm::mat4 &view, const glm::mat4 &projection) {
        glEnable(GL_BLEND);
        // **KLÍČOVÁ ZMĚNA: Normální alfa blending pro kouř**
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        // Můžeme znovu použít stejný shader jako pro oheň!
        auto smokeShader = this->resourceManager.getShader("fire");
        smokeShader->use();
        smokeShader->setMat4("view", view);
        smokeShader->setMat4("projection", projection);

        // Zvětšíme částice kouře, aby byl efekt plnější
        smokeShader->setFloat("particleSize", 0.008f);

        // Použijeme texturu kouře
        this->resourceManager.getTexture("smoke.png")->bind();

        std::vector<FireParticle> liveParticles;
        liveParticles.reserve(maxParticles);
        for (const auto &p: particles) {
            if (p.life > 0.0f) {
                liveParticles.push_back(p);
            }
        }

        if (!liveParticles.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, liveParticles.size() * sizeof(FireParticle), liveParticles.data());
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, liveParticles.size());
            glBindVertexArray(0);
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void SmokeParticleSystem::addParticle() {
        for (int i = 0; i < maxParticles; ++i) {
            int index = (lastUsedParticle + i) % maxParticles;
            if (particles[index].life <= 0.0f) {
                FireParticle &p = particles[index];

                // Rodí se na stejném místě jako oheň
                glm::vec2 spawnDisk = glm::diskRand(0.015f);
                p.position = glm::vec3(spawnDisk.x,3.0f, -0.8f);

                // Rychlost kouře je pomalejší a více se rozptyluje
                p.velocity.x = glm::linearRand(-0.005f, 0.005f);
                p.velocity.y = glm::linearRand(0.04f, 0.07f); // Stoupá pomaleji než oheň
                p.velocity.z = glm::linearRand(-0.05f, 0.05f);

                // Barva kouře - tmavě šedá s náhodnou světlostí
                float greyTone = glm::linearRand(0.2f, 0.4f);
                p.color = glm::vec4(greyTone, greyTone, greyTone, 0.7f); // Začíná poloprůhledný
                p.life = 4.0f; // Žije déle než oheň
                lastUsedParticle = index;
                return;
            }
        }
    }
} // Particle
