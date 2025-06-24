#ifndef SMOKEPARTICLESYSTEM_H
#define SMOKEPARTICLESYSTEM_H

#include "../Manager/TextureManager.h"
#include "../Manager/ResourceManager.h"

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
        SmokeParticleSystem(ResourceManager *resourceManager, const glm::vec3 &origin, const glm::mat4 &proj);
        void update(float deltaTime);
        void draw(const glm::mat4 &viewProj);

    private:
        void respawnParticle(SmokeParticle &particle);
        glm::mat4 projection{};
        std::vector<SmokeParticle> particles;
        glm::vec3 origin;

        ShaderManager* shader;
        TextureManager* texture;
        // std::shared_ptr<TextureManager> texture;

        unsigned int VAO{}, VBO{};
        ResourceManager *resourceManager;
    };
}

#endif //SMOKEPARTICLESYSTEM_H
