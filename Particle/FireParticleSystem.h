#ifndef FIREPARTICLESYSTEM_H
#define FIREPARTICLESYSTEM_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "../Manager/TextureManager.h"
#include "../Manager/ResourceManager.h"

namespace Particle {
    struct FireParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
        glm::vec4 color;
    };

    class FireParticleSystem {
    public:
        FireParticleSystem(ResourceManager *resourceManager, const glm::vec3 &origin, glm::mat4 proj);

        void update(float deltaTime);

        void draw(const glm::mat4 &viewProj);

    private:
        void respawnParticle(FireParticle &particle);
        glm::mat4 projection{};
        std::vector<FireParticle> particles;
        glm::vec3 origin;

        ShaderManager* shader;
        TextureManager* texture;
        // std::shared_ptr<TextureManager> texture;

        unsigned int VAO{}, VBO{};
        ResourceManager *resourceManager;
    };
} // Particle

#endif //FIREPARTICLESYSTEM_H
