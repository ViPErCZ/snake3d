#include "GPUParticle3D.h"

namespace Model {
    GPUParticle3D::GPUParticle3D(const shared_ptr<Camera> &camera, const shared_ptr<StandardMesh> &mesh,
                                 const shared_ptr<ResourceManager> &resourceManager,
                                 const int maxParticles)
        : MeshNode3D(mesh, resourceManager), resourceManager(resourceManager), maxParticles(maxParticles), VAO{},
          particleVBO{}, camera(camera) {
        initBuffers();
        setPreset(Preset::Fire);
        setRenderMode(RenderMode::Color);
    }

    void GPUParticle3D::update(const float dt, const uint64_t frameId) {
        if (lastUpdatedFrame == frameId) {
            return;
        }

        lastUpdatedFrame = frameId;
        auto runTfStep = [&](const float stepDt) {
            const int src = frameIndex % 2;
            const int dst = (frameIndex + 1) % 2;
            const auto shader = resourceManager->getShader("particle_update");
            shader->use();
            shader->setFloat("u_dt", stepDt);
            shader->setFloat("u_timeAccum", timeAccum);
            shader->setVec3("u_emitterPos", (particleParams.spawnShape == 1) ? camera->getPosition() : glm::vec3(0.0f));

            // Nové uniformy
            shader->setInt("u_spawnShape", particleParams.spawnShape);
            shader->setInt("u_respawnMode", particleParams.respawnMode);
            shader->setVec2("u_turbulence", particleParams.turbulence);
            shader->setFloat("u_minRadius", particleParams.minRadius);
            shader->setFloat("u_maxRadius", particleParams.maxRadius);
            shader->setFloat("u_spawnHeight", particleParams.spawnHeight);
            // Obecné uniformy z ParticleParams (parametrizace TF výstupu)
            shader->setInt("u_mode", particleParams.mode);
            shader->setFloat("u_lifeMin", particleParams.lifeMin);
            shader->setFloat("u_lifeMax", particleParams.lifeMax);
            shader->setFloat("u_sizeMin", particleParams.sizeMin);
            shader->setFloat("u_sizeMax", particleParams.sizeMax);
            shader->setVec3("u_velMin", particleParams.velMin);
            shader->setVec3("u_velMax", particleParams.velMax);
            shader->setVec3("u_gravity", particleParams.gravity);
            shader->setFloat("u_emitterRadius", particleParams.emitterRadius);
            shader->setFloat("u_emitterRadiusX", particleParams.emitterRadiusX);
            shader->setFloat("u_emitterRadiusZ", particleParams.emitterRadiusZ);
            shader->setFloat("u_emitterYOffset", particleParams.emitterYOffset);
            shader->setFloat("u_spawnPerFrame", particleParams.spawnPerFrame);

            glEnable(GL_RASTERIZER_DISCARD);
            glBindVertexArray(VAO[src]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleVBO[dst]);
            glBeginTransformFeedback(GL_POINTS);
            glDrawArrays(GL_POINTS, 0, maxParticles);
            glEndTransformFeedback();
            glDisable(GL_RASTERIZER_DISCARD);

            frameIndex++;
        };

        if (firstFrame && particleParams.smoothStart) {
            const int substeps = std::max(1, particleParams.warmupSubsteps);
            const float totalWarmup = std::max(0.0f, particleParams.warmupTime);
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
            usedDt = std::min(dt, particleParams.firstFrameClamp);
            firstFrame = false;
        }

        runTfStep(usedDt);
        timeAccum += usedDt;
    }

    void GPUParticle3D::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) {

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        const bool useTexture = (renderMode == RenderMode::Textured) && !particleParams.texture.empty();
        const auto shader = resourceManager->getShader(useTexture ? "instanced_texture" : "instanced_mesh");
        shader->use();
        shader->setMat4("view", camera->getViewMatrix());
        shader->setMat4("projection", projection);
        if (currentPreset != Preset::Rain && currentPreset != Preset::Snow) {
            shader->setMat4("model", parentTransform * this->getModelMatrix());
        } else {
            // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
            shader->setMat4("model", glm::mat4(1.0f));
        }
        shader->setInt("u_mode", particleParams.mode);
        shader->setFloat("u_lifeMin", particleParams.lifeMin);
        shader->setFloat("u_lifeMax", particleParams.lifeMax);
        shader->setFloat("u_sizeMin", particleParams.sizeMin);
        shader->setFloat("u_sizeMax", particleParams.sizeMax);
        shader->setFloat("u_stretch", particleParams.stretch);
        shader->setVec4("u_colorStart", particleParams.colorStart);
        shader->setVec4("u_colorEnd", particleParams.colorEnd);
        shader->setFloat("u_colorSensitivity", particleParams.colorSensitivity);
        if (useTexture) {
            shader->setInt("uTexture0", 0);
            const auto tex = resourceManager->getTexture(particleParams.texture);
            if (tex) tex->bind();
        }

        mesh->bind();

        // Připravit instanced atributy (stav) pro aktuální src buffer
        const int src = frameIndex % 2; // pozor: update už frameIndex zvýšil, proto zde aktuální src
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO[src]);
        // iPos @location 4
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, position));
        glVertexAttribDivisor(4, 1);
        // iVel @location 5
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, velocity));
        glVertexAttribDivisor(5, 1);
        // iLife @location 6
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, life));
        glVertexAttribDivisor(6, 1);
        // iSeed @location 7
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)offsetof(GPUParticle, seed));
        glVertexAttribDivisor(7, 1);

        glDrawElementsInstanced(
            GL_TRIANGLES,
            static_cast<GLsizei>(mesh->indicesCount()),
            GL_UNSIGNED_INT,
            nullptr,
            maxParticles
        );

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void GPUParticle3D::initBuffers() {
        std::vector<GPUParticle> initial(maxParticles);
        for (int i = 0; i < maxParticles; i++) {
            initial[i].position = glm::vec3(0.0f);
            initial[i].velocity = glm::vec3(0.0f);
            //initial[i].life = 0.0f;
            initial[i].seed = static_cast<float>(i) * 17.123f;

            const float maxL = particleParams.lifeMax > 0 ? particleParams.lifeMax : 2.0f;
            initial[i].life = static_cast<float>(random()) / static_cast<float>(RAND_MAX) * maxL;
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

    void GPUParticle3D::setPreset(const Preset preset) {
        currentPreset = preset;
        particleParams.spawnShape = 0; // Local
        particleParams.respawnMode = 0; // Die
        particleParams.turbulence = {0.0f, 0.0f};
        particleParams.minRadius = 0.0f;

        switch (preset) {
            case Preset::Fire: {
                particleParams.lifeMin = 1.4f; particleParams.lifeMax = 1.6f;
                particleParams.sizeMin = 0.006f; particleParams.sizeMax = 0.020f;
                particleParams.stretch = 0.32f;
                particleParams.velMin = {-0.005f, 0.000f, 0.100f};
                particleParams.velMax = { 0.005f, 0.010f, 0.200f};
                particleParams.gravity = {0.0f, 0.0f, -0.15f};
                particleParams.emitterRadius = 0.015f;
                particleParams.emitterYOffset = 0.046f;
                particleParams.colorStart = {6.0f, 3.5f, 1.0f, 1.0f};
                particleParams.colorEnd   = {7.0f, 4.5f, 1.5f, 0.0f};
                particleParams.mode = Stretched;
                particleParams.minRadius = 0.05f;
                break;
            }
            case Preset::Smoke: {
                particleParams.lifeMin = 2.0f; particleParams.lifeMax = 4.0f;
                particleParams.sizeMin = 0.05f; particleParams.sizeMax = 0.2f;
                particleParams.velMin = {-0.05f, 0.2f, -0.05f};
                particleParams.velMax = { 0.05f, 0.6f,  0.05f};
                particleParams.gravity = {0.0f, -0.05f, 0.0f};
                particleParams.emitterRadius = 0.08f;
                particleParams.colorStart = {0.4f, 0.4f, 0.4f, 0.8f};
                particleParams.colorEnd   = {0.2f, 0.2f, 0.2f, 0.0f};
                particleParams.mode = Stretched;
                particleParams.minRadius = 0.05f;
                break;
            }
            case Preset::Rain: {
                particleParams.lifeMin = 1.0f;
                particleParams.lifeMax = 1.8f;
                particleParams.sizeMin = 0.012f;
                particleParams.sizeMax = 0.012f;
                particleParams.stretch = 0.05f;
                particleParams.stretch = 0.003f;
                particleParams.velMin = { -0.2f, -0.2f, -15.0f };
                particleParams.velMax = {  0.2f,  0.2f, -25.0f };
                particleParams.gravity = { 0.0f, 0.0f, -2.8f };
                particleParams.emitterRadius = 25.0f;
                particleParams.emitterYOffset = 15.0f;
                particleParams.colorStart = { 0.25f, 0.35f, 0.8f, 0.45f };
                particleParams.colorEnd   = { 0.25f, 0.35f, 0.8f, 0.45f };
                particleParams.colorSensitivity = 10.0f;
                particleParams.mode = Billboard;
                particleParams.spawnShape = 1;  // Environment (kolem kamery)
                particleParams.respawnMode = 1; // Infinite wrap
                particleParams.turbulence = {0.0f, 0.0f}; // ŽÁDNÉ KLOUZÁNÍ!

                particleParams.minRadius = 3.0f;  // 3m díra kolem kamery
                particleParams.maxRadius = 30.0f; // 30m dohlednost
                particleParams.spawnHeight = 25.0f;
                break;
            }
            case Preset::Snow: {
                particleParams.lifeMin = 1.0f;
                particleParams.lifeMax = 12.0f;
                particleParams.sizeMin = 0.04f;
                particleParams.sizeMax = 0.12f;
                particleParams.stretch = 0.0f;
                particleParams.velMin = { -1.5f, -1.5f, -0.8f };
                particleParams.velMax = {  1.5f,  1.5f, -1.8f };
                particleParams.gravity = { 0.0f, 0.0f, -0.5f };
                particleParams.emitterRadius = 30.0f;
                particleParams.emitterYOffset = 10.0f;
                particleParams.colorStart = { 1.0f, 1.0f, 1.0f, 1.0f };
                particleParams.colorEnd   = { 1.0f, 1.0f, 1.0f, 0.0f };
                particleParams.colorSensitivity = 2.0f;
                particleParams.spawnPerFrame = 0;
                particleParams.mode = Billboard;
                particleParams.spawnShape = 1;
                particleParams.respawnMode = 1;
                particleParams.turbulence = {0.5f, 0.8f}; // SÍLA KLOUZÁNÍ

                particleParams.minRadius = 4.0f;
                particleParams.maxRadius = 25.0f;
                particleParams.spawnHeight = 20.0f;
                break;
            }
            case Preset::Explosion: {
                particleParams.lifeMin = 0.4f; particleParams.lifeMax = 0.9f;
                particleParams.sizeMin = 0.03f; particleParams.sizeMax = 0.12f;
                particleParams.velMin = {-3.0f, -1.0f, -3.0f};
                particleParams.velMax = { 3.0f,  3.0f,  3.0f};
                particleParams.gravity = {0.0f, -4.0f, 0.0f};
                particleParams.emitterRadius = 0.02f;
                particleParams.colorStart = {8.0f, 5.0f, 2.0f, 1.0f};
                particleParams.colorEnd   = {2.0f, 1.0f, 0.2f, 0.0f};
                particleParams.mode = Stretched;
                break;
            }
            case Preset::Custom:
            default:
                break;
        }
    }
} // Model
