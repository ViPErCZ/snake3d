#include "SpinnerModel.h"
#include <vector>

namespace Model {
    SpinnerModel::SpinnerModel(const shared_ptr<BaseItem> &baseItem, shared_ptr<ShaderManager> &baseShader)
        : StandardMesh(baseItem, baseShader), time(0) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices = {0, 1, 2};

        Vertex v1{}, v2{}, v3{};

        v1.position = {0.0f, 0.025f, 0.0f};
        v2.position = {-0.01f, -0.025f, 0.0f};
        v3.position = {0.01f, -0.025f, 0.0f};

        v1.normal = v2.normal = v3.normal = {0.0f, 0.0f, 1.0f};
        v1.texUV = v2.texUV = v3.texUV = {0.0f, 0.0f};

        vertices = {v1, v2, v3};

        mesh = std::make_shared<Mesh>(vertices, indices);

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

    void SpinnerModel::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt) const {
        baseShader->use();
        mesh->bind();

        baseShader->setUniform("view", camera->getViewMatrix());
        baseShader->setUniform("projection", projection);

        for (int i = 0; i < numInstances; ++i) {
            const float progress = static_cast<float>(i) / static_cast<float>(numInstances);
            const float angle = progress * glm::two_pi<float>() * spiralTurns - time;
            const float radius = 0.1f + progress * 0.04f;
            glm::vec3 position(radius * cos(angle), radius * sin(angle), 0.0f);
            item->setPosition(position);

            const float scale = 0.5f + progress * 0.05f;

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, angle - glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(scale));

            float hue = fmod(progress - time * 0.2f, 1.0f);
            if (hue < 0.0f) hue += 1.0f;

            baseShader->setUniform("model", model);
            baseShader->setUniform("objectColor", hsvToRgb(hue, 0.9f, 1.0f));

            glLoadIdentity();
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);
        }
    }

    void SpinnerModel::update(const float dt) {
        time += dt * speed;
    }

    glm::vec3 SpinnerModel::hsvToRgb(const float h, const float s, float v) {
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
