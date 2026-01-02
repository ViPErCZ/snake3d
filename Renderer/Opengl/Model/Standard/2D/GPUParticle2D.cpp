#include "GPUParticle2D.h"

namespace Model {

    GPUParticle2D::GPUParticle2D(const shared_ptr<ContextState> &contextState, const shared_ptr<BaseNode2D> &mesh,
        const shared_ptr<ResourceManager> &resourceManager, const int maxParticles)
        : MeshNode2D(contextState, mesh, resourceManager), maxParticles(maxParticles) {

        initBuffers();
        setPreset(Preset::RainOnGlass);
    }

    GPUParticle2D::~GPUParticle2D() {
        glDeleteVertexArrays(2, VAO);
        glDeleteBuffers(2, VBO);
    }

    void GPUParticle2D::update(const float dt, const uint64_t frameId) {
        if (lastUpdatedFrame == frameId) return;
        lastUpdatedFrame = frameId;

        // Transform Feedback update
        const auto shader = resourceManager->getShader("particle_update_2d");
        shader->use();

        const int src = frameIndex % 2;
        const int dst = (frameIndex + 1) % 2;

        shader->setFloat("u_dt", dt);
        shader->setFloat("u_time", timeAccum);
        shader->setFloat("u_seed_iter", static_cast<float>(frameId % 1000) / 1000.0f);

        // Params pass
        shader->setVec2("u_emitterPos", params.emitterPos);
        shader->setVec2("u_emitterSize", params.emitterSize);
        shader->setVec2("u_gravity", params.gravity);
        shader->setVec2("u_velMin", params.velocityMin);
        shader->setVec2("u_velMax", params.velocityMax);
        shader->setFloat("u_drag", params.drag);             // Důležité pro sklo!
        shader->setFloat("u_turbulence", params.turbulence);

        shader->setFloat("u_lifeMin", params.lifeMin);
        shader->setFloat("u_lifeMax", params.lifeMax);
        shader->setInt("u_spawnMode", params.spawnMode);

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
        // glDisable(GL_DEPTH_TEST);
        // glEnable(GL_BLEND);
        //
        // if (currentPreset == Preset::MagicFire) {
        //     glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        // } else {
        //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // }

        contextState->setBlendingMode(mesh->getBlending());
        contextState->setDepthTest(mesh->getDepthTest());
        contextState->setDepthWrite(mesh->getDepthWrite());

        const bool useTexture = !params.texture.empty();
        const auto shader = resourceManager->getShader(useTexture ? "particle_render_2d_tex" : "particle_render_2d");
        shader->use();

        shader->setFloat("u_aspectRatio", /* aspectRatio */ 1.77f);
        shader->setVec4("u_colorStart", params.colorStart);
        shader->setVec4("u_colorEnd", params.colorEnd);
        shader->setFloat("u_sizeMin", params.sizeMin);
        shader->setFloat("u_sizeMax", params.sizeMax);
        shader->setFloat("u_time", timeAccum);

        // Textura jednotka 0: SCÉNA (pozadí)
        resourceManager->getTexture("SceneTexture")->bind();
        shader->setInt("uSceneTexture", 0);

        if (useTexture) {
            shader->setInt("uNormalTexture", 1);
            auto tex = resourceManager->getTexture(params.texture);
            if (tex) tex->bind(1);
        }

        // Bind Quad Mesh (Předpokládám, že máš sdílený quad v manageru)
        // Pokud ne, můžeš ho vytvořit lokálně nebo použít glDrawArrays bez bufferu (shader trick)
        mesh->bind();

        const int src = frameIndex % 2;
        glBindBuffer(GL_ARRAY_BUFFER, VBO[src]);

        // Instancing Attribute Setup
        // Předpokládám, že Mesh má základní vertex data na lokacích 0, 1, 2...
        // My přidáme instanced data na vyšší lokace (např. 3, 4, 5, 6)

        // 1. Position (vec2) -> Location 3
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
            reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, position)));
        glVertexAttribDivisor(3, 1);

        // 2. Velocity (vec2) -> Location 4 (pro motion blur nebo orientaci)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct),
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

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    void GPUParticle2D::setPreset(const Preset preset) {
        currentPreset = preset;
        // Reset params
        params = ParticleParams2D();

        switch (preset) {
            case Preset::RainOnGlass:
                params.spawnMode = 1;
                params.emitterSize = {2.0f, 2.0f}; // -1..1 pokrytí
                params.gravity = {0.0f, -0.5f}; // Padá dolů
                params.drag = 2.0f;
                params.turbulence = 0.2f;

                params.lifeMin = 2.0f; params.lifeMax = 5.0f;
                params.sizeMin = 0.009f; params.sizeMax = 0.03f;
                params.colorStart = {0.8f, 0.9f, 1.0f, 0.3f};
                params.colorEnd = {0.8f, 0.9f, 1.0f, 0.0f};
                params.texture = "drop_normal.png";

                params.velocityMin = {0.0f, -0.1f};
                params.velocityMax = {0.0f, -0.8f}; // Některé jedou rychle
                break;

            case Preset::MagicFire:
                params.spawnMode = 0; // Point
                params.emitterPos = {0.0f, -0.8f}; // Dole uprostřed
                params.gravity = {0.0f, 0.8f}; // Stoupá
                params.lifeMin = 0.5f; params.lifeMax = 1.2f;
                params.colorStart = {1.0f, 0.5f, 0.1f, 1.0f};
                params.colorEnd = {0.2f, 0.0f, 0.0f, 0.0f};
                params.sizeMin = 0.1f; params.sizeMax = 0.0f;
                break;

            default: break;
        }
    }

    void GPUParticle2D::initBuffers() {
        std::vector<GPUParticle2DStruct> initial(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            // Začínají mimo obrazovku nebo na random místě
            initial[i].position = glm::vec2(-2.0f, -2.0f);
            initial[i].velocity = glm::vec2(0.0f);
            initial[i].life = 0.0f; // Mrtvé
            initial[i].seed = static_cast<float>(random()) / static_cast<float>(RAND_MAX);
        }

        glGenVertexArrays(2, VAO);
        glGenBuffers(2, VBO);

        for (int i = 0; i < 2; i++) {
            glBindVertexArray(VAO[i]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
            glBufferData(GL_ARRAY_BUFFER, maxParticles * sizeof(GPUParticle2DStruct), initial.data(), GL_DYNAMIC_COPY);

            // Nastavení pro Transform Feedback (čtení v update shaderu)
            // Layout: 0=Pos, 1=Vel, 2=Life, 3=Seed
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct), static_cast<void *>(nullptr));

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct), reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, velocity)));

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct), reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, life)));

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle2DStruct), reinterpret_cast<void *>(offsetof(GPUParticle2DStruct, seed)));
        }
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

}