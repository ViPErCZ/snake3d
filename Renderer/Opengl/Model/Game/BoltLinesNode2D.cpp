#include "../Game/BoltLinesNode2D.h"

#include <GLFW/glfw3.h>

namespace Model {
    BoltLinesNode2D::BoltLinesNode2D(const glm::mat4 &perspectiveProjection, const shared_ptr<ShaderManager> &shader)
        : BaseNode2D(shader), perspectiveProj(perspectiveProjection) {
        setBlending(Blending::AlphaAdditive);
        setDepthTest(false);
        setDepthWrite(false);
        initGeometry();
    }

    BoltLinesNode2D::~BoltLinesNode2D() {
        if (linesVAO != 0) glDeleteVertexArrays(1, &linesVAO);
        if (linesVBO != 0) glDeleteBuffers(1, &linesVBO);
    }

    void BoltLinesNode2D::initGeometry() {
        constexpr float vertices[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
        glGenVertexArrays(1, &linesVAO);
        glGenBuffers(1, &linesVBO);
        glBindVertexArray(linesVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linesVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void BoltLinesNode2D::setSegments(const vector<Segment> &segs) {
        segments = segs;
    }

    void BoltLinesNode2D::clearSegments() {
        segments.clear();
    }

    void BoltLinesNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                                  const glm::mat4 &parentTransform) const {
        if (segments.empty() || !baseShader) return;

        glLineWidth(3.0f);

        baseShader->use();
        baseShader->setMat4("projection", perspectiveProj);
        baseShader->setMat4("view", camera->getViewMatrix());
        baseShader->setFloat("time", static_cast<float>(glfwGetTime()));

        for (const auto &seg : segments) {
            baseShader->setVec3("startPos", seg.start);
            baseShader->setVec3("endPos", seg.end);
            baseShader->setFloat("brightness", seg.brightness * 2.0f);
            glBindVertexArray(linesVAO);
            glDrawArrays(GL_LINES, 0, 2);
        }

        glLineWidth(1.0f);
    }
} // Model
