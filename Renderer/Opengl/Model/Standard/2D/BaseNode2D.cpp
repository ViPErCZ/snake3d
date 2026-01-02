#include "BaseNode2D.h"

#include "../../../Material/StandardMaterial.h"

namespace Model {
    BaseNode2D::BaseNode2D(const shared_ptr<ShaderManager> &baseShader) : baseShader(baseShader), color(1.0f) {
    }

    void BaseNode2D::setMaterial(const shared_ptr<BaseMaterial> &material) {
        this->material = material;
    }

    void BaseNode2D::setColor(const glm::vec3 &color) {
        this->color = color;
    }

    void BaseNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                            const glm::mat4 &parentTransform) const {

        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                ortho,
                parentTransform,
                false
            );
        } else if (baseShader) {
            baseShader->use();
            baseShader->setMat4("projection", ortho);
            baseShader->setMat4("model", parentTransform);
            baseShader->setVec3("color", color);
            baseShader->setInt("textureMap", 0);
            baseShader->setBool("useMaterial", true);
            baseShader->setFloat("alpha", 1.0);
            baseShader->setFloat("expansion", 1.0);
        }

        mesh->bind();
        glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                       nullptr);
        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->unbind();
        }
    }

    void BaseNode2D::setBlending(const Blending blending) {
        this->blending = blending;
    }

    void BaseNode2D::setDepthTest(const bool depthTest) {
        this->depthTest = depthTest;
    }

    bool BaseNode2D::getDepthTest() const {
        return depthTest;
    }

    void BaseNode2D::setDepthWrite(const bool depthWrite) {
        this->depthWrite = depthWrite;
    }

    bool BaseNode2D::getDepthWrite() const {
        return depthWrite;
    }

    Blending BaseNode2D::getBlending() const {
        return blending;
    }

    void BaseNode2D::bind() const {
        mesh->bind();
    }

    unsigned long BaseNode2D::indicesCount() const {
        return mesh->getIndices().size();
    }
} // Model