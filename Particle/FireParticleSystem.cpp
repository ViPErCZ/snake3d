#include "FireParticleSystem.h"
#include <cstdlib>

namespace Particle {
    FireParticleSystem::FireParticleSystem(ResourceManager *resourceManager, const glm::vec3 &origin, glm::mat4 proj)
        : resourceManager(resourceManager), origin(origin), projection(proj) {
        shader = resourceManager->getShader("fire");
        texture = resourceManager->getTexture("fire.png");

        particles.resize(100);
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(FireParticle), particles.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (void*)offsetof(FireParticle, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (void*)offsetof(FireParticle, color));
        glBindVertexArray(0);

        for (auto& p : particles)
            respawnParticle(p);
    }

    void FireParticleSystem::respawnParticle(FireParticle &p) {
        p.position = origin + glm::vec3(((rand() % 100) - 50) / 100.0f, 0.0f, ((rand() % 100) - 50) / 100.0f);
        p.velocity = glm::vec3(0.0f, 1.0f + (rand() % 100) / 100.0f, 0.0f);
        p.life = 1.0f;
        p.color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
    }

    void FireParticleSystem::update(float dt) {
        for (auto &p: particles) {
            p.life -= dt;
            if (p.life <= 0.0f) {
                respawnParticle(p);
            }
            p.position += p.velocity * dt;
            p.color.a = p.life;
        }
    }

    void FireParticleSystem::draw(const glm::mat4 &viewProj) {
        shader->use();
        shader->setMat4("uViewProj", glm::mat4(1.0f));
        texture->bind(0);
        shader->setInt("uTexture", 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        // Update particle data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(FireParticle), particles.data());

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, particles.size());
        glBindVertexArray(0);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
} // Particle
