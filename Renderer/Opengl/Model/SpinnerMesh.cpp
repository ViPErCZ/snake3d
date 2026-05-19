#include "SpinnerMesh.h"
#include <vector>

namespace Model {
    SpinnerMesh::SpinnerMesh(const shared_ptr<ShaderProgram> &baseShader)
        : TringleMesh3D(baseShader, 0.04, 0.04), time(0) {
        colors = {
            {1.0f, 0.0f, 0.0f},
            {1.0f, 0.5f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 0.5f, 1.0f},
            {0.5f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f}
        };
    }

    void SpinnerMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                            const glm::mat4 &parentTransform, bool shadows) const {
        baseShader->use();
        baseShader->setUniform("view", camera->getViewMatrix());
        baseShader->setUniform("projection", projection);
        mesh->bind();

        for (int i = 0; i < numInstances; ++i) {
            const float progress = static_cast<float>(i) / static_cast<float>(numInstances);
            const float angle = progress * glm::two_pi<float>() * spiralTurns - time;
            const float radius = 0.1f + progress * 0.04f;
            glm::vec3 position(radius * cos(angle), radius * sin(angle), 0.0f);

            const float scale = 0.5f + progress * 0.05f;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, angle - glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(scale));

            float hue = fmod(progress - time * 0.2f, 1.0f);
            if (hue < 0.0f) hue += 1.0f;

            baseShader->setUniform("model", parentTransform * model);
            baseShader->setUniform("objectColor", hsvToRgb(hue, 0.9f, 1.0f));

            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                       nullptr);
        }
    }

    void SpinnerMesh::update(const float dt) {
        time += dt * speed;
    }

    glm::vec3 SpinnerMesh::hsvToRgb(const float h, const float s, float v) {
        if (s == 0.0f) return {v, v, v};
        int i = static_cast<int>(h * 6.0f);
        const float f = h * 6.0f - static_cast<float>(i);
        float p = v * (1.0f - s);
        float q = v * (1.0f - f * s);
        float t = v * (1.0f - (1.0f - f) * s);
        i %= 6;
        if (i == 0) return {v, t, p};
        if (i == 1) return {q, v, p};
        if (i == 2) return {p, v, t};
        if (i == 3) return {p, q, v};
        if (i == 4) return {t, p, v};
        return {v, p, q};
    }

} // Model
