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

        // Pos: location 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (void*)offsetof(FireParticle, position));

        // Color: location 1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FireParticle), (void*)offsetof(FireParticle, color));

        glBindVertexArray(0);

        for (auto &p: particles) {
            respawnParticle(p);
        }
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
        shader->setMat4("uViewProj", projection * viewProj);
        texture->bind(0);
        shader->setInt("uTexture", 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_PROGRAM_POINT_SIZE);

        std::cout << "VAO bound: " << VAO << ", VBO: " << VBO << std::endl;

        glBindVertexArray(VAO);
        GLint currentVAO = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
        std::cout << "Currently bound VAO: " << currentVAO << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(FireParticle), particles.data());

        GLint bufferSize;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
        std::cout << "VBO size: " << bufferSize << ", expected: " << particles.size() * sizeof(FireParticle) << std::endl;
        GLint posLoc = glGetAttribLocation(shader->getId(), "aPos");
        GLint colLoc = glGetAttribLocation(shader->getId(), "aColor");
        std::cout << "aPos loc: " << posLoc << ", aColor loc: " << colLoc << std::endl;

        glDrawArrays(GL_POINTS, 0, particles.size());
        glBindVertexArray(0);

        glDisable(GL_BLEND);

        std::cout << "Drawing " << particles.size() << " particles." << std::endl;
        while (auto err = glGetError()) {
            std::cout << "GL Error after draw: " << std::hex << err << std::dec << "\n";
        }
    }
} // Particle
