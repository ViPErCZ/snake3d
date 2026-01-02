#ifndef SNAKE3_GPUPARTICLE2D_H
#define SNAKE3_GPUPARTICLE2D_H

#include <memory>

#include "../../../../../Manager/ResourceManager.h"
#include "MeshNode2D.h"

using namespace std;

namespace Model {

    struct GPUParticle2DStruct {
        glm::vec2 position;
        glm::vec2 velocity;
        float life;
        float seed;
    };

    struct ParticleParams2D {
        // Emitter
        glm::vec2 emitterPos = {0.0f, 0.0f};
        glm::vec2 emitterSize = {0.0f, 0.0f}; // x = šířka, y = výška
        float spawnRate = 1.0f; // Kolik částic za frame/čas

        // Fyzika
        glm::vec2 gravity = {0.0f, -1.0f};
        glm::vec2 velocityMin = {0.0f, 0.0f};
        glm::vec2 velocityMax = {0.0f, 0.0f};
        float drag = 0.0f;       // Odpor vzduchu/skla (pro zastavení kapek)
        float turbulence = 0.0f; // Náhodný pohyb do stran

        // Vzhled
        float lifeMin = 1.0f;
        float lifeMax = 2.0f;
        float sizeMin = 0.01f;
        float sizeMax = 0.05f;
        glm::vec4 colorStart = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 colorEnd = {1.0f, 1.0f, 1.0f, 0.0f};
        std::string texture;

        // Logika
        int spawnMode = 0; // 0 = Point, 1 = Rectangle (Top/Screen)
    };

    class GPUParticle2D : public MeshNode2D {
    public:
        enum class Preset { RainOnGlass, MagicFire, Snow2D, Custom };

        explicit GPUParticle2D(const shared_ptr<ContextState> &contextState,
            const shared_ptr<BaseNode2D> &mesh, const shared_ptr<ResourceManager> &resourceManager, int maxParticles);

        ~GPUParticle2D() override;

        void update(float dt, uint64_t frameId) override;
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const override; // aspectRatio pro zachování čtvercovosti

        void setPreset(Preset preset);
        ParticleParams2D& getParams() { return params; }

    private:
        void initBuffers();

        int maxParticles;

        // OpenGL handles
        GLuint VAO[2]{};
        GLuint VBO[2]{}; // Position, Vel, Life, Seed

        // Data
        ParticleParams2D params;
        Preset currentPreset{};

        // State
        int frameIndex = 0;
        uint64_t lastUpdatedFrame = 0;
        float timeAccum = 0.0f;
        bool firstFrame = true;
    };
}

#endif //SNAKE3_GPUPARTICLE2D_H