#include "LabelNode2D.h"

#include <utility>
#include "../../../../../Manager/VboIndexer.h"
#include "../../Utils/TextMesh.h"

namespace Model {
    LabelNode2D::LabelNode2D(std::string text,
                             const std::shared_ptr<ShaderManager> &baseShader,
                             const std::shared_ptr<LabelSettings> &settings)
        : BaseNode2D(baseShader), settings(settings), text(std::move(text)) {
        if (!settings->getFont()) {
            std::cerr << "LabelNode2D: Font is null!" << std::endl;
            return;
        }

        mesh = std::make_shared<TextMesh>();
        textureId = settings->getFont()->getAtlasTextureId();
    }

    void LabelNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
        const glm::mat4 &parentTransform) const {

        const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(3, mesh->getSizeY(), 0.0f));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        baseShader->use();
        baseShader->setMat4("projection", ortho);
        baseShader->setMat4("model", model);
        baseShader->setVec3("textColor", glm::vec3(1.0f));
        baseShader->setInt("textTexture", 0);
        baseShader->setFloat("alpha", 1.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        mesh->bind(text, settings->getFont());
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(mesh->getVertices().size())/4);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);
    }
} // namespace Model
