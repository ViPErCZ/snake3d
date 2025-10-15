#ifndef FIREPARTICLESYSTEM_H
#define FIREPARTICLESYSTEM_H

#include <vector>
#include "../Manager/ResourceManager.h"

using namespace Manager;

// FireParticle struct (zůstává stejná)
struct FireParticle {
    glm::vec3 position, velocity;
    glm::vec4 color;
    float life;
};

class FireParticleSystem {
public:
    FireParticleSystem(ResourceManager& resourceManager, int maxParticles);

    void update(float dt, glm::vec3 offset);
    void render(const glm::mat4& view, const glm::mat4& projection) const;

private:
    void init();
    void addParticle(glm::vec3 offset);

    std::vector<FireParticle> particles;
    int maxParticles;

    // VAO a VBO buffery jsou nyní členské proměnné
    unsigned int VAO{};
    unsigned int quadVBO{}, instanceVBO{};

    ResourceManager& resourceManager;
    int lastUsedParticle = 0;
};

#endif //FIREPARTICLESYSTEM_H
