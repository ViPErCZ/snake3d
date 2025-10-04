#include "FireParticleSystem.h"
#include <cstdlib>
#include <glm/gtc/random.hpp>

// Konstruktor si uloží referenci na ResourceManager
FireParticleSystem::FireParticleSystem(ResourceManager& resourceManager, const int maxParticles)
    : maxParticles(maxParticles), resourceManager(resourceManager) {
    init();
}

void FireParticleSystem::init() {
    particles.resize(maxParticles);

    // Vytvoření VAO a VBO pro quad (geometrie částice)
    float quadVertices[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 1.0f };
    const unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));

    // **ZDE JE TA OPRAVA**
    // Buffer pro instancovaná data (pozice, barvy, atd.) se vytvoří POUZE JEDNOU ZDE
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // Alokujeme paměť pro všechny částice, ale zatím ji neplníme (nullptr)
    glBufferData(GL_ARRAY_BUFFER, sizeof(FireParticle) * maxParticles, nullptr, GL_DYNAMIC_DRAW);

    // A také atributy nastavíme POUZE JEDNOU ZDE
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FireParticle), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FireParticle), reinterpret_cast<void *>(offsetof(FireParticle, color)));
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
}

void FireParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    const auto fireShader = this->resourceManager.getShader("fire");
    fireShader->use();
    fireShader->setMat4("view", view);
    fireShader->setMat4("projection", projection);

    fireShader->setFloat("particleSize", 0.008f);

    this->resourceManager.getTexture("fire.png")->bind();

    std::vector<FireParticle> liveParticles;
    liveParticles.reserve(maxParticles);
    for (const auto& p : particles) {
        if (p.life > 0.0f) {
            liveParticles.push_back(p);
        }
    }

    if (!liveParticles.empty()) {
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, liveParticles.size() * sizeof(FireParticle), liveParticles.data());

        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, liveParticles.size());
        glBindVertexArray(0);
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void FireParticleSystem::update(float dt, glm::vec3 offset) {
    // Každý snímek přidáme několik nových částic pro plynulý efekt
    for(int i = 0; i < 5; ++i) {
        addParticle(offset);
    }

    for (int i = 0; i < maxParticles; ++i) {
        FireParticle& p = particles[i];
        if (p.life > 0.0f) {
            p.life -= dt;
            p.position += p.velocity * dt;

            // Plynulý přechod barev a mizení
            float lifeRatio = p.life / 1.5f;

            p.color.r = glm::mix(2.0f, 7.0f, lifeRatio); // Červená složka od 2.0 do 7.0
            p.color.g = glm::mix(0.5f, 4.5f, lifeRatio); // Zelená složka od 0.5 do 4.5
            p.color.b = glm::mix(0.1f, 1.5f, lifeRatio); // Modrá složka jen lehce

            p.color.a = glm::smoothstep(0.0f, 0.8f, lifeRatio);
        }
    }
}

void FireParticleSystem::addParticle(const glm::vec3 offset) {
    // Najdeme první "mrtvou" částici, kterou můžeme znovu použít
    for (int i = 0; i < maxParticles; ++i) {
        int index = (lastUsedParticle + i) % maxParticles;
        if (particles[index].life <= 0.0f) {
            FireParticle& p = particles[index];

            // Rodí se v menší oblasti (poloměr 0.15f místo 0.3f)
            glm::vec2 spawnDisk = glm::diskRand(0.015f);
            //p.position = glm::vec3(0.038 + spawnDisk.x, -0.012f, -0.76f);
            p.position = glm::vec3(spawnDisk.x, 0.046f, spawnDisk.y) + offset;

            // Rychlost je menší, aby oheň nebyl tak vysoký
            p.velocity.x = glm::linearRand(-0.005f, 0.005f); // Menší rozptyl do stran
            p.velocity.y = glm::linearRand(0.01f, 0.001f);   // Menší rychlost nahoru
            p.velocity.z = glm::linearRand(0.05f, 0.09f);

            p.color = glm::vec4(6.0f, 3.5f, 1.0f, 1.0f);
            p.life = 1.5f; // Životnost
            lastUsedParticle = index;
            return; // Našli jsme volné místo, končíme
        }
    }
    // Pokud jsme nenašli žádné volné místo, tak tento snímek žádnou částici nepřidáme.
}

