#ifndef FIRERENDERER_H
#define FIRERENDERER_H
#include "BaseRenderer.h"
#include "../../Particle/SmokeParticleSystem.h"

using namespace Particle;
using namespace std;
using namespace Manager;

namespace Renderer {

    class FireRenderer : public BaseRenderer {
    public:
        FireRenderer(Camera *camera, const glm::mat4 &projection, ResourceManager *resManager);
        ~FireRenderer() override;
        void render(float dt) override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override;
    protected:
        FireParticleSystem *fires;
        SmokeParticleSystem *smokes;
        Camera* camera;
        glm::mat4 projection{};
        glm::vec3 offset{};
    };
} // Renderer

#endif //FIRERENDERER_H
