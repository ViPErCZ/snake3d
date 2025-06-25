#include "FireRenderer.h"

namespace Renderer {
    FireRenderer::FireRenderer(Camera *camera, const glm::mat4 &projection, ResourceManager *resManager):
        camera(camera), projection(projection)
    {
        fires = new FireParticleSystem(*resManager, 500); // 500 = počet částic
        smokes = new SmokeParticleSystem(*resManager, 500); // 500 = počet částic
    }

    FireRenderer::~FireRenderer() {
        delete fires;
        delete smokes;
    }

    void FireRenderer::render(const float dt) {
        fires->update(dt);
        smokes->update(dt, glm::vec3(0.0f, 3.0f, -0.8f));
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