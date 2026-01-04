#include "GPUParticle3D.h"

namespace Model {
    GPUParticle3D::GPUParticle3D(const shared_ptr<ParticleProcessMaterial> &material,
        const shared_ptr<ContextState> &contextState,
        const shared_ptr<Camera> &camera, const shared_ptr<StandardMesh> &mesh,
        const shared_ptr<ResourceManager> &resourceManager, const int maxParticles)
        : MeshNode3D(contextState, mesh, resourceManager), VAO{},
          particleVBO{}, maxParticles(maxParticles), resourceManager(resourceManager), camera(camera), material(material) {
        update_shader = resourceManager->getShader("particle_update");
        render_shader = resourceManager->getShader("particle_3d_render");
        render_texture_shader = resourceManager->getShader("particle_3d_render_tex");
        initBuffers();
    }

    GPUParticle3D::~GPUParticle3D() {
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, particleVBO);
    }

    void GPUParticle3D::update(const float dt, const uint64_t frameId) {
        if (lastUpdatedFrame == frameId) {
            return;
        }

        lastUpdatedFrame = frameId;
        auto runTfStep = [&](const float stepDt) {
            const int src = frameIndex % 2;
            const int dst = (frameIndex + 1) % 2;

            if (material) {
                if (material->get_spawn_shape() == 1) {
                    material->set_emitter_pos(camera->getPosition());
                }
                material->update(update_shader, maxParticles, timeAccum, timeOffset, stepDt);
                frameIndex++;
            } else {
                throw std::invalid_argument("GPUParticle Process Material missing.");
            }

            glEnable(GL_RASTERIZER_DISCARD);
            glBindVertexArray(VAO[src]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBO[dst]);
            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, maxParticles);
            glEndTransformFeedback();
            glDisable(GL_RASTERIZER_DISCARD);
        };

        if (firstFrame && material->is_smooth_start()) {
            const int substeps = std::max(1, material->get_warmup_substeps());
            const float totalWarmup = std::max(0.0f, material->get_warmup_time());
            const float subDt = (substeps > 0 && totalWarmup > 0.0f) ? (totalWarmup / static_cast<float>(substeps)) : 0.0f;

            for (int i = 0; i < substeps; ++i) {
                runTfStep(subDt);
                timeAccum += subDt;
            }
            firstFrame = false;
            return;
        }

        float usedDt = dt;
        if (firstFrame) {
            usedDt = std::min(dt, material->get_first_frame_clamp());
            firstFrame = false;
        }

        runTfStep(usedDt);
        timeAccum += usedDt;
    }

    void GPUParticle3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, const bool shadows) {

        const glm::mat4 finalTransform = parentTransform * this->getModelMatrix();
        contextState->setBlendingMode(mesh->getBlending());
        contextState->setDepthTest(mesh->getDepthTest());
        contextState->setDepthWrite(mesh->getDepthWrite());

        if (material) {
            const auto shader = !material->get_texture().empty() ? render_texture_shader : render_shader;
            material->bind(shader);
            shader->setMat4("view", camera->getViewMatrix());
            shader->setMat4("projection", projection);
            shader->setMat4("model", material->get_mode() == Billboard ? glm::mat4(1.0f) : finalTransform);
        } else {
            throw std::invalid_argument("GPUParticle Process Material missing.");
        }

        mesh->bind();

        const int src = frameIndex % 2;
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO[src]);
        // iPos @location 4
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), reinterpret_cast<void *>(offsetof(GPUParticle, position)));
        glVertexAttribDivisor(4, 1);
        // iVel @location 5
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), reinterpret_cast<void *>(offsetof(GPUParticle, velocity)));
        glVertexAttribDivisor(5, 1);
        // iLife @location 6
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), reinterpret_cast<void *>(offsetof(GPUParticle, life)));
        glVertexAttribDivisor(6, 1);
        // iSeed @location 7
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), reinterpret_cast<void *>(offsetof(GPUParticle, seed)));
        glVertexAttribDivisor(7, 1);

        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(mesh->indicesCount()),
            GL_UNSIGNED_INT,
            nullptr,
            maxParticles
        );
    }

    void GPUParticle3D::initBuffers() {
        std::vector<GPUParticle> initial(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            initial[i].position = glm::vec3(0.0f);
            initial[i].velocity = glm::vec3(0.0f);
            initial[i].seed = static_cast<float>(i) * 17.123f;

            const float maxL = material->get_life_max() > 0 ? material->get_life_max() : 2.0f;
            initial[i].life = static_cast<float>(random()) / static_cast<float>(RAND_MAX) * maxL;
        }

        // Ping-pong buffer pro TF
        glGenVertexArrays(2, VAO);
        glGenBuffers(2, particleVBO);

        for (int i = 0; i < 2; i++) {
            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, particleVBO[i]);
            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizei>(maxParticles * sizeof(GPUParticle)),
                         initial.data(),
                         GL_DYNAMIC_COPY);

            glEnableVertexAttribArray(0); // inPos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  reinterpret_cast<void *>(offsetof(GPUParticle, position)));

            glEnableVertexAttribArray(1); // inVel
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  reinterpret_cast<void *>(offsetof(GPUParticle, velocity)));

            glEnableVertexAttribArray(2); // inLife
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  reinterpret_cast<void *>(offsetof(GPUParticle, life)));

            glEnableVertexAttribArray(3); // inSeed
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  reinterpret_cast<void *>(offsetof(GPUParticle, seed)));
        }

        glBindVertexArray(0);

        // Unbind VAO
        glBindVertexArray(0);
    }
} // Model
