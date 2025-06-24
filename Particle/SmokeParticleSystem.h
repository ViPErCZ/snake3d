#ifndef SMOKEPARTICLESYSTEM_H
#define SMOKEPARTICLESYSTEM_H

#include "FireParticleSystem.h"

namespace Particle {
    struct SmokeParticle {
        glm::vec3 position;  // 12 B
        float size;          // 4 B → dorovnáme první 16B blok

        glm::vec4 color;     // 16 B → zarovnáno

        glm::vec3 velocity;  // 12 B
        float life;          // 4 B → dorovná 16 B
    };

    class SmokeParticleSystem {
    public:
        SmokeParticleSystem(ResourceManager& resourceManager, int maxParticles);

        void update(float dt);
        void render(const glm::mat4& view, const glm::mat4& projection);

    private:
        void init();
        void addParticle();

        std::vector<FireParticle> particles;
        int maxParticles;

        unsigned int VAO{};
        unsigned int quadVBO{}, instanceVBO{};

        ResourceManager& resourceManager;
        int lastUsedParticle = 0;
    };
}

#endif //SMOKEPARTICLESYSTEM_H
