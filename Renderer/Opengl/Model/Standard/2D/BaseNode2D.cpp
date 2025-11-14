#include "BaseNode2D.h"

#include "../../../Material/StandardMaterial.h"

namespace Model {
    BaseNode2D::BaseNode2D(const shared_ptr<ShaderManager> &baseShader) : baseShader(baseShader) {
    }

    void BaseNode2D::setMaterial(const shared_ptr<BaseMaterial> &material) {
        this->material = material;
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
        } else {
            baseShader->use();
            baseShader->setMat4("projection", ortho);
            baseShader->setMat4("model", parentTransform);
            baseShader->setVec3("textColor", glm::vec3(1.0f));
            baseShader->setInt("textTexture", 0);
            baseShader->setFloat("alpha", 1.0);
        }

        mesh->bind();
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(mesh->getVertices().size())/4);
        glBindVertexArray(0);
    }
} // Model