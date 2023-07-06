#ifndef SNAKE3_RAINMODEL_H
#define SNAKE3_RAINMODEL_H

#include "Utils/Mesh.h"
#include <random>

using namespace Manager;
using namespace ModelUtils;

namespace Model {

    struct Particle {
        glm::vec3 Position, Velocity;
        glm::vec4 Color;
        Particle() : Position(0.0f, 0.0f, -3.0f), Velocity(0.4f), Color(1.0f) { }
    };

    class RainModel {
    public:
        explicit RainModel(BaseItem *baseItem, unsigned int amount);
        virtual ~RainModel();
        void update(float dt, unsigned int newParticles);
        [[nodiscard]] Mesh *getMesh() const;
        [[nodiscard]] const vector<Particle> &getParticles() const;

    protected:
        static void respawnParticle(Particle &particle);
        unsigned int firstUnusedParticle();
        unsigned int amount;
        unsigned int lastUsedParticle;
        Mesh* mesh;
        BaseItem* item;
        std::vector<Particle> particles;
    };
}

#endif //SNAKE3_RAINMODEL_H
