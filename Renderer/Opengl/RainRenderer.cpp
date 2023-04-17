#include "RainRenderer.h"

Renderer::RainRenderer::RainRenderer(BaseItem *item, Camera *camera, glm::mat4 proj, ResourceManager *resourceManager)
        : BaseRenderer(item), resourceManager(resourceManager), camera(camera), projection(proj), enable(false) {
    baseShader = resourceManager->getShader("rain");
    texture = resourceManager->getTexture("rain.jpg");
    model = new RainModel(item, 6000);
}

Renderer::RainRenderer::~RainRenderer() {
    delete model;
}

void Renderer::RainRenderer::render() {
    if (enable) {
        baseShader->use();
        for (Particle particle: model->getParticles()) {
            if (particle.Position.z >= -2.0f) {
                baseShader->setVec2("offset", particle.Position);
                baseShader->setVec4("color", particle.Color);
                baseShader->setInt("sprite", 0);
                baseShader->setMat4("projection", projection);
                glm::mat4 tr = glm::mat4(1.0f);
                // Transform the matrices to their correct form
                tr = glm::translate(tr, {0.0, 0.0, 0.0});
                tr = glm::translate(tr, particle.Position);
                tr = glm::rotate(tr, glm::radians(90.0f), {1.0, 0.0, 0.0f});
                baseShader->setMat4("model", tr);
                baseShader->setMat4("view", camera->getViewMatrix());
                texture->bind(0);

                model->getMesh()->bind();
                glDrawElements(GL_TRIANGLES, (int) model->getMesh()->getIndices().size(), GL_UNSIGNED_INT, nullptr);
                glBindVertexArray(0);
            }
        }
    }
}

void Renderer::RainRenderer::renderShadowMap() {

}

void Renderer::RainRenderer::beforeRender() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_DST_COLOR);
    auto currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    if (currentFrame > lastFrame) {
        item->setPosition({0.0, 0, -2.5});
        model->update(deltaTime, 130);
        lastFrame = currentFrame;
    }
}

void Renderer::RainRenderer::afterRender() {
    // don't forget to reset to default blending mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
}

void Renderer::RainRenderer::toggle() {
    enable = !enable;
}

bool Renderer::RainRenderer::isEnable() const {
    return enable;
}
