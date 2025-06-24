#ifndef SMOKEPARTICLESYSTEM_H
#define SMOKEPARTICLESYSTEM_H

#include "FireParticleSystem.h"

namespace Particle {
    // Vytvoříme novou, bohatší strukturu pro částice kouře
    struct SmokeParticle {
        glm::vec3 position, velocity;
        glm::vec4 color;
        float life;
        float size;
        float rotation;
        float rotationSpeed;
    };

    class SmokeParticleSystem {
    public:
        SmokeParticleSystem(ResourceManager& resourceManager, int maxParticles);
        void update(float dt, glm::vec3 offset);
        void render(const glm::mat4& view, const glm::mat4& projection);
    private:
        void init();
        void addParticle(glm::vec3 offset);

        std::vector<SmokeParticle> particles;
        int maxParticles;

        unsigned int VAO{};
        unsigned int quadVBO{}, instanceVBO{};

        ResourceManager& resourceManager;
        int lastUsedParticle = 0;
    };
}

#endif //SMOKEPARTICLESYSTEM_H
