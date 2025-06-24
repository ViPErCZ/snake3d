#include "SmokeParticleSystem.h"

namespace Particle {
    SmokeParticleSystem::SmokeParticleSystem(ResourceManager *resourceManager, const glm::vec3 &origin, const glm::mat4 &proj)
        : projection(proj), origin(origin), resourceManager(resourceManager) {
        shader = resourceManager->getShader("smoke");
        texture = resourceManager->getTexture("smoke.png");

        particles.resize(200);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(SmokeParticle), particles.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (void*)offsetof(SmokeParticle, position));
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (void*)offsetof(SmokeParticle, color));
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (void*)offsetof(SmokeParticle, size));

        glBindVertexArray(0);

        for (auto &p: particles)
            respawnParticle(p);
    }

    void SmokeParticleSystem::respawnParticle(SmokeParticle &p) {
        p.position = origin + glm::vec3(((rand() % 100) - 50) / 800.0f, 0.0f, ((rand() % 100) - 50) / 800.0f);
        p.velocity = glm::vec3(
            ((rand() % 100) - 50) / 100.0f,  // lehké horizontální kolísání
            0.5f + (rand() % 50) / 100.0f,   // pomalejší stoupání
            ((rand() % 100) - 50) / 100.0f
        );
        p.life = 0.5f + (rand() % 200) / 200.0f * 0.4f;
        p.color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f); // nebo až 0.1f pro temnější kouř
        p.size = 32; //glm::mix(0.5f, 2.5f, 1.0f - p.life);
    }

    void SmokeParticleSystem::update(float dt) {
        // Omezíme maximum, aby se první frame "nerozbil"
        dt = std::min(dt, 0.05f);

        for (auto &p: particles) {
            p.life -= dt;
            if (p.life <= 0.0f) respawnParticle(p);

            p.position += p.velocity * dt + glm::vec3(0.0f, dt * 0.2f, 0.0f);
            p.position.x += sin(p.life * 20.0f) * dt * 0.1f;
            p.position.z += cos(p.life * 20.0f) * dt * 0.1f;
            p.color = glm::vec4(1.0f, 0.8f * p.life, 0.0f, p.life);
            p.size = glm::mix(1.5f, 16.5f, 1.0f - p.life);
        }
    }

    void SmokeParticleSystem::draw(const glm::mat4 &viewProj) {
        shader->use();
        shader->setMat4("uViewProj", projection * viewProj * glm::mat4(1.0f));
        texture->bind(0);
        shader->setInt("uTexture", 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        // Update particle data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(SmokeParticle), particles.data());

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, particles.size());
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
} // Particle