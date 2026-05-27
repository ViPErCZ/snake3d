#ifndef SNAKE3_GPUPARTICLE2D_H
#define SNAKE3_GPUPARTICLE2D_H

#include <memory>

#include "../../../../../Manager/ResourceManager.h"
#include "MeshNode2D.h"
#include "../../../Material/Particle/ParticleProcessMaterial.h"

namespace Model {
    using std::shared_ptr;

    struct GPUParticle2DStruct {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
        float seed;
    };

    class GPUParticle2D : public MeshNode2D {
    public:
        explicit GPUParticle2D(const shared_ptr<Material::ParticleProcessMaterial> &material,
            const shared_ptr<Tools::ContextState> &contextState,
                               const shared_ptr<BaseNode2D> &mesh, const shared_ptr<Manager::ResourceManager> &resourceManager,
                               int maxParticles);

        ~GPUParticle2D() override;

        void update(float dt, uint64_t frameId) override;

        void render(const shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const override;

        void setTimeOffset(const float timeOffset) { this->timeOffset = timeOffset; }
        void setAspectRatio(const float ratio) { this->aspectRatio = ratio; }
        void setRenderShader(const shared_ptr<Manager::ShaderProgram> &shader) { render_shader = shader; }
        void setRenderTextureShader(const shared_ptr<Manager::ShaderProgram> &shader) { render_texture_shader = shader; }

    private:
        void initBuffers();

        GLuint VAO[2]{};
        GLuint VBO[2]{};

        int frameIndex = 0;
        float timeAccum = 0.0f;
        bool firstFrame = true;
        float timeOffset = 0.0f;
        float aspectRatio = 1.6f;
        int maxParticles;

        shared_ptr<Material::ParticleProcessMaterial> material;
        shared_ptr<Manager::ShaderProgram> update_shader;
        shared_ptr<Manager::ShaderProgram> render_shader;
        shared_ptr<Manager::ShaderProgram> render_texture_shader;
    };
}

#endif //SNAKE3_GPUPARTICLE2D_H
