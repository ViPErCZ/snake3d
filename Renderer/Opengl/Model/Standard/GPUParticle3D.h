#ifndef SNAKE3_GPUPARTICLE3D_H
#define SNAKE3_GPUPARTICLE3D_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "MeshNode3D.h"
#include "../../../../Manager/Camera.h"
#include "../../../../Manager/ResourceManager.h"

using namespace Manager;

namespace Model {

    struct GPUParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
        float seed;
    };

    struct ParticleInstance {
        glm::mat4 model;
        glm::vec4 color;
    };

    class GPUParticle3D : public MeshNode3D {
    public:
        GPUParticle3D(const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager,
            int maxParticles);

        void update(float dt) override;
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;
    private:
        void initBuffers();

        shared_ptr<ResourceManager> resourceManager;
        int maxParticles;

        GLuint VAO[2];
        GLuint meshVAO{};
        GLuint particleVBO[2];
        GLuint meshVBO{};
        GLuint particleInstanceVBO{};

        int frameIndex = 0;
    };
} // Model

#endif //SNAKE3_GPUPARTICLE3D_H