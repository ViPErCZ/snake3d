#include "GPUParticle3D.h"

namespace Model {
    GPUParticle3D::GPUParticle3D(const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager,
                                 const int maxParticles) : MeshNode3D(mesh, resourceManager),
                                                           resourceManager(resourceManager), maxParticles(maxParticles),
                                                           VAO{}, particleVBO{} {
        initBuffers();
    }

    void GPUParticle3D::update(const float dt) {
        const int src = frameIndex % 2;
        const auto shader = resourceManager->getShader("particle_update");
        shader->use();
        shader->setFloat("u_dt", dt);
        shader->setVec3("u_emitterPos", getPosition());

        glEnable(GL_RASTERIZER_DISCARD);

        glBindVertexArray(VAO[src]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleInstanceVBO);

        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, maxParticles);
        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD);

        frameIndex++;
    }

    void GPUParticle3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        const auto shader = resourceManager->getShader("instanced_mesh");
        shader->use();
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", projection);

        mesh->bind();

        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(mesh->indicesCount()),
            GL_UNSIGNED_INT,
            nullptr,
            maxParticles
        );

        glDisable(GL_BLEND);
    }

    void GPUParticle3D::initBuffers() {
        // Inicializace "simulačních" částic pro TF
        std::vector<GPUParticle> initial(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            initial[i].position = glm::vec3(0.0f);
            initial[i].velocity = glm::vec3(0.0f);
            initial[i].life = 0.0f;
            initial[i].seed = static_cast<float>(i) * 17.123f;
        }

        // Ping-pong buffer pro TF
        glGenVertexArrays(2, VAO);
        glGenBuffers(2, particleVBO);

        for (int i = 0; i < 2; i++) {
            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, particleVBO[i]);
            glBufferData(GL_ARRAY_BUFFER,
                         maxParticles * sizeof(GPUParticle),
                         initial.data(),
                         GL_DYNAMIC_COPY);

            // TF atributy (inPos, inVel, inLife, inSeed)
            glEnableVertexAttribArray(0); // inPos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  (void *) offsetof(GPUParticle, position));

            glEnableVertexAttribArray(1); // inVel
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  (void *) offsetof(GPUParticle, velocity));

            glEnableVertexAttribArray(2); // inLife
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  (void *) offsetof(GPUParticle, life));

            glEnableVertexAttribArray(3); // inSeed
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
                                  sizeof(GPUParticle),
                                  (void *) offsetof(GPUParticle, seed));
        }

        glBindVertexArray(0);

        // Instance buffer (model matrix + color)
        glGenBuffers(1, &particleInstanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, particleInstanceVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     maxParticles * sizeof(ParticleInstance),
                     nullptr,
                     GL_DYNAMIC_DRAW); // TF bude zapisovat sem

        // Pro diagnostiku: naplň pár instancí CPU daty, aby se dalo ověřit kreslení bez TF
        {
            const int initCount = std::min(maxParticles, 8);
            std::vector<ParticleInstance> tmp(initCount);
            for (int i = 0; i < initCount; ++i) {
                tmp[i].model = glm::mat4(1.0f);
                // Rozprostřít několik jednotkových instancí do řady, ať jsou vidět
                tmp[i].model[3] = glm::vec4(float(i) * 0.15f, 0.0f, 0.0f, 1.0f);
                tmp[i].color = glm::vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
            glBufferSubData(GL_ARRAY_BUFFER, 0, initCount * sizeof(ParticleInstance), tmp.data());
        }

        // Navázání instancovaných atributů na VAO meshe
        // Musíme bindnout VAO, které používá samotný mesh
        mesh->bind();
        // POZOR: mesh->bind() typicky přebinduje GL_ARRAY_BUFFER na VBO meshe.
        // Aby instanced atributy ukazovaly do správného bufferu, je nutné znovu bindnout particleInstanceVBO.
        glBindBuffer(GL_ARRAY_BUFFER, particleInstanceVBO);

        std::size_t offset = 0;
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(4 + i); // location 4,5,6,7 pro mat4 (po řádcích)
            glVertexAttribPointer(
                4 + i, 4, GL_FLOAT, GL_FALSE,
                sizeof(ParticleInstance),
                (void *)(offset)
            );
            glVertexAttribDivisor(4 + i, 1);
            offset += sizeof(glm::vec4);
        }

        // Color (vec4) na location 8
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(
            8, 4, GL_FLOAT, GL_FALSE,
            sizeof(ParticleInstance),
            (void *)offsetof(ParticleInstance, color)
        );
        glVertexAttribDivisor(8, 1);

        // Unbind VAO
        glBindVertexArray(0);
    }
} // Model
