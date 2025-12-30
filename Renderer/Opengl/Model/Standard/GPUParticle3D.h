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

    struct ParticleStateOut {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
        float seed;
    };

    class GPUParticle3D : public MeshNode3D {
    public:
        GPUParticle3D(const shared_ptr<Camera> &camera,
            const shared_ptr<StandardMesh> &mesh, const shared_ptr<ResourceManager> &resourceManager,
            int maxParticles);

        void update(float dt, uint64_t frameId) override;
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                    const glm::mat4 &parentTransform, bool shadows) override;

        enum class Preset {
            Fire,
            Smoke,
            Rain,
            Snow,
            Explosion,
            Custom
        };

        enum class RenderMode {
            Color,
            Textured
        };

        enum ParticleMode {
            Mesh3D = 2,
            Billboard = 1,
            Stretched = 0
        };

        struct ParticleParams {
            // životnost a velikost
            float lifeMin = 1.0f;
            float lifeMax = 2.0f;
            float sizeMin = 0.02f;
            float sizeMax = 0.08f;
            // natažení billboardu podle rychlosti (sy = s + |vel| * stretch)
            float stretch = 0.15f;
            // rychlost a gravitace
            glm::vec3 velMin = {-0.1f, 0.6f, -0.1f};
            glm::vec3 velMax = { 0.1f, 1.6f,  0.1f};
            glm::vec3 gravity = {0.0f, -0.4f, 0.0f};
            // emitor
            float emitterRadius = 0.05f; // pro kruhový emitor (XZ) – nastaví obě osy níže
            // eliptický/kruhový emitor v rovině XZ (Z‑up)
            float emitterRadiusX = 0.05f;
            float emitterRadiusZ = 0.05f;
            float emitterYOffset = 0.0f; // vertikální posun základu emitoru (např. 0.046 pro oheň)
            // barvy
            glm::vec4 colorStart = {1.0f, 1.0f, 1.0f, 1.0f};
            glm::vec4 colorEnd   = {1.0f, 1.0f, 1.0f, 0.0f};
            // rendering
            std::string texture;
            // hustota spawnování (efektivní) – frakční část použita jako pravděpodobnost okamžitého respawnu
            float spawnPerFrame = 1.0f;
            // plynulý start simulace (eliminuje startovní „výstřel“)
            bool smoothStart = true;
            float warmupTime = 2.5f;       // doba náběhu (s)
            int   warmupSubsteps = 4;      // počet dílčích kroků v prvním snímku
            float firstFrameClamp = 1.0f/30.0f; // maximální dt na prvním snímku
            float colorSensitivity = 1.0f;
            ParticleMode mode = Billboard;
            int spawnShape = 0;      // 0 = Local, 1 = Environment Ring, 2 = Sphere
            int respawnMode = 0;     // 0 = Die, 1 = Wrap infinite
            glm::vec2 turbulence = {0.0f, 0.0f}; // x=sila, y=frekvence
            float minRadius = 0.0f;  // vnitrni polomer (safe zone)
            float maxRadius = 1.0f;  // vnejsi polomer (area)
            float spawnHeight = 10.0f; // vyska sloupce
            float timeOffset = 0.0f;
        };

        void setPreset(Preset preset);
        void setParams(const ParticleParams& params) { particleParams = params; syncDerivedParams(); }
        const ParticleParams& getParams() const { return particleParams; }
        void setRenderMode(const RenderMode mode) { renderMode = mode; }
    private:
        void initBuffers();
        void syncDerivedParams() {
            // udržet zpětnou kompatibilitu: emitterRadius je zkratka pro X i Z, pokud nejsou explicitně změněny
            particleParams.emitterRadiusX = particleParams.emitterRadius;
            particleParams.emitterRadiusZ = particleParams.emitterRadius;
        }

        shared_ptr<ResourceManager> resourceManager;
        int maxParticles;

        GLuint VAO[2];
        GLuint meshVAO{};
        GLuint particleVBO[2];
        GLuint meshVBO{};
        GLuint particleInstanceVBO{};
        GLuint stateTFVBO[2]{};

        int frameIndex = 0;
        bool firstFrame = true;
        float timeAccum = 0.0f;

        Preset currentPreset = Preset::Custom;
        RenderMode renderMode = RenderMode::Color;
        ParticleParams particleParams{};
        shared_ptr<Camera> camera;
    };
} // Model

#endif //SNAKE3_GPUPARTICLE3D_H