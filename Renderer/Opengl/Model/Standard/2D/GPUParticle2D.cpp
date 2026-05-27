#include "GPUParticle2D.h"

using namespace std;
using namespace Manager;
using namespace Material;
using namespace Tools;

namespace Model {

    GPUParticle2D::GPUParticle2D(const shared_ptr<ParticleProcessMaterial> &material,
        const shared_ptr<ContextState> &contextState, const shared_ptr<BaseNode2D> &mesh,
        const shared_ptr<ResourceManager> &resourceManager, const int maxParticles)
        : MeshNode2D(contextState, mesh, resourceManager), maxParticles(maxParticles), material(material) {
        update_shader = resourceManager->getShader("particle_update_2d");
        render_shader = resourceManager->getShader("particle_render_2d");
        render_texture_shader = resourceManager->getShader("particle_render_2d_tex");
        initBuffers();
    }

    GPUParticle2D::~GPUParticle2D() {
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, VBO);
    }

    void GPUParticle2D::update(const float dt, const uint64_t frameId) {
        if (lastUpdatedFrame == frameId) return;
        lastUpdatedFrame = frameId;

        update_shader->use();

        const int src = frameIndex % 2;
        const int dst = (frameIndex + 1) % 2;

        update_shader->setFloat("u_dt", dt);
        update_shader->setFloat("u_timeAccum", timeAccum);
        update_shader->setBool("u_is2D", true);

        if (material) {
            material->update(update_shader, maxParticles, timeAccum, timeOffset, dt);
        } else {
            throw std::invalid_argument("GPUParticle Process Material missing.");
        }

        // Transform Feedback
        glEnable(GL_RASTERIZER_DISCARD);
        glBindVertexArray(VAO[src]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[dst]);

        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, maxParticles);
        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
        glBindVertexArray(0);

        frameIndex++;
        timeAccum += dt;
        firstFrame = false;
    }

    void GPUParticle2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const {
        contextState->setBlendingMode(mesh->getBlending());
        contextState->setDepthTest(mesh->getDepthTest());
        contextState->setDepthWrite(mesh->getDepthWrite());

        if (material) {
            const auto shader = !material->get_texture().empty() ? render_texture_shader : render_shader;
            material->bind(shader);
            shader->setFloat("u_aspectRatio", aspectRatio);
            resourceManager->getTexture("SceneTexture")->bind(1);
            shader->setInt("uSceneTexture", 1);
        } else {
            throw std::invalid_argument("GPUParticle Process Material missing.");
        }

        mesh->bind();

        const int src = frameIndex % 2;
        glBindBuffer(GL_ARRAY_BUFFER, VBO[src]);

        // 1. Position (vec3) -> Location 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
            reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, position)));
        glVertexAttribDivisor(3, 1);

        // 2. Velocity (vec3) -> Location 4 (pro motion blur nebo orientaci)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
            reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, velocity)));
        glVertexAttribDivisor(4, 1);

        // 3. Life (float) -> Location 5
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
            reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, life)));
        glVertexAttribDivisor(5, 1);

        // 4. Seed (float) -> Location 6 (pro random variace v render shaderu)
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
            reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, seed)));
        glVertexAttribDivisor(6, 1);

        glDrawElementsInstanced(GL_TRIANGLES,
            static_cast<int>(mesh->indicesCount()),
            GL_UNSIGNED_INT, nullptr, maxParticles);

        // Cleanup
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
        glDisableVertexAttribArray(6);
    }

    void GPUParticle2D::initBuffers() {
        std::vector<GPUParticle2DStruct> initial(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            initial[i].position = glm::vec3(-2.0f, -2.0f, 0.0f);
            initial[i].velocity = glm::vec3(0.0f);
            initial[i].life = 0.0f;
            initial[i].seed = static_cast<float>(random()) / static_cast<float>(RAND_MAX);
        }

        glGenVertexArrays(2, VAO);
        glGenBuffers(2, VBO);

        for (int i = 0; i < 2; i++) {
            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(maxParticles * sizeof(GPUParticle2DStruct)), initial.data(), GL_DYNAMIC_COPY);

            // Layout: 0=Pos, 1=Vel, 2=Life, 3=Seed
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,sizeof(GPUParticle2DStruct),
                reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, position))
            );

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
                                  reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, velocity)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
                                  reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, life)));

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
                                  reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, seed)));
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}