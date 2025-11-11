#include "BaseNode2D.h"

namespace Model {
    BaseNode2D::BaseNode2D(const shared_ptr<ShaderManager> &baseShader) : baseShader(baseShader) {
    }

    void BaseNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
        const glm::mat4 &parentTransform) const {

        const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(3, 100, 0.0f));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        baseShader->use();
        baseShader->setMat4("projection", ortho);
        // baseShader->setMat4("model", model);
        baseShader->setVec3("textColor", glm::vec3(0.5f));
        baseShader->setInt("textTexture", 0);
        baseShader->setFloat("alpha", 1.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        // mesh->bind();
        // //glDrawArrays(GL_TRIANGLES, 0, mesh->getVerticesSize());
        //  glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
        //                 nullptr);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);
    }
} // Model