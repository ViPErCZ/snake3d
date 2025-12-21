#include "FireRenderer.h"

namespace Renderer {
    FireRenderer::FireRenderer(Camera *camera, const glm::mat4 &projection, ResourceManager *resManager):
        camera(camera), projection(projection)
    {
        fires = new FireParticleSystem(*resManager, 500); // 500 = počet částic
        smokes = new SmokeParticleSystem(*resManager, 500); // 500 = počet částic
        offset = glm::vec3(0.038, -0.012f, -0.76f);
    }

    FireRenderer::~FireRenderer() {
        delete fires;
        delete smokes;
    }

    void FireRenderer::render3D(const float dt, uint64_t frameId) {
        fires->update(dt, offset);
        smokes->update(dt, offset);
        fires->render(camera->getViewMatrix(), projection);
        smokes->render(camera->getViewMatrix(), projection);
    }

    void FireRenderer::beforeRender() {
    }

    void FireRenderer::afterRender() {
    }

    void FireRenderer::renderShadowMap() {
    }
} // Renderer