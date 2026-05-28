#ifndef SNAKE3_GPUPARTICLE3D_H
#define SNAKE3_GPUPARTICLE3D_H

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "MeshNode3D.h"
#include <snake3d/Manager/Camera.h>
#include <snake3d/Manager/ResourceManager.h>
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
        GPUParticle3D(const std::shared_ptr<Material::ParticleProcessMaterial> &material,
            const std::shared_ptr<Tools::ContextState> &contextState, const std::shared_ptr<Manager::Camera> &camera,
            const std::shared_ptr<StandardMesh> &mesh, const std::shared_ptr<Manager::ResourceManager> &resourceManager, int maxParticles);

        ~GPUParticle3D() override;

        void update(float dt, uint64_t frameId) override;

        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
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

        std::shared_ptr<Manager::ResourceManager> resourceManager;
        std::shared_ptr<Manager::Camera> camera;
        std::shared_ptr<Material::ParticleProcessMaterial> material;
        std::shared_ptr<Manager::ShaderProgram> update_shader;
        std::shared_ptr<Manager::ShaderProgram> render_shader;
        std::shared_ptr<Manager::ShaderProgram> render_texture_shader;
    };
} // Model

#endif //SNAKE3_GPUPARTICLE3D_H
