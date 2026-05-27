#ifndef SNAKE3_GPUPARTICLE3D_H
#define SNAKE3_GPUPARTICLE3D_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "MeshNode3D.h"
#include "../../../../Manager/Camera.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../Material/Particle/ParticleProcessMaterial.h"

namespace Model {

    struct GPUParticle {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
        float seed;
    };

    class GPUParticle3D : public MeshNode3D {
    public:
        GPUParticle3D(const shared_ptr<Material::ParticleProcessMaterial> &material,
            const shared_ptr<ContextState> &contextState, const shared_ptr<Manager::Camera> &camera,
            const shared_ptr<StandardMesh> &mesh, const shared_ptr<Manager::ResourceManager> &resourceManager, int maxParticles);

        ~GPUParticle3D() override;

        void update(float dt, uint64_t frameId) override;

        void render(const shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        void setTimeOffset(const float timeOffset) { this->timeOffset = timeOffset; }
        void restart(bool startDead = true);
        void setTimeScale(const float scale) { timeScale = scale; }

    private:
        void initBuffers(bool startDead);
        void refillBuffers(bool startDead);

        GLuint VAO[2];
        GLuint meshVAO{};
        GLuint particleVBO[2];
        GLuint meshVBO{};
        GLuint particleInstanceVBO{};
        GLuint stateTFVBO[2]{};

        int maxParticles;
        int frameIndex = 0;
        bool firstFrame = true;
        float timeAccum = 0.0f;
        float timeOffset = 0.0f;
        float timeScale = 1.0f;

        shared_ptr<Manager::ResourceManager> resourceManager;
        shared_ptr<Manager::Camera> camera;
        shared_ptr<Material::ParticleProcessMaterial> material;
        shared_ptr<ShaderProgram> update_shader;
        shared_ptr<ShaderProgram> render_shader;
        shared_ptr<ShaderProgram> render_texture_shader;
    };
} // Model

#endif //SNAKE3_GPUPARTICLE3D_H
