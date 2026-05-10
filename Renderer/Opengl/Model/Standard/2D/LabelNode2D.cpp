#include "LabelNode2D.h"

#include <utility>
#include <iostream>
#include "../../../../../Manager/VboIndexer.h"
#include "../../../Material/StandardMaterial.h"
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
        mesh->update(this->text, settings->getFont());
        textureId = settings->getFont()->getAtlasTextureId();
        blending = Blending::Text;
    }

    void LabelNode2D::setText(const string &text) {
        this->text = text;
        mesh->update(text, settings->getFont());
        align = glm::vec2(3.0f, mesh->getSizeY());
    }

    void LabelNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                             const glm::mat4 &parentTransform) const {

        const glm::mat4 model = translate(parentTransform, glm::vec3(align.x, align.y, 0.0f));

        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                ortho,
                model,
                false
            );
        } else {
            baseShader->use();
            baseShader->setMat4("projection", ortho);
            baseShader->setMat4("model", model);
            baseShader->setVec3("textColor", settings->getColor());
            baseShader->setInt("textTexture", 0);
            baseShader->setFloat("alpha", 1.0);
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        mesh->bind();
        glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(mesh->getVertices().size())/4);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D,0);
    }

    void LabelNode2D::alignVerticalCenter(const float viewportWidth, const float viewportHeight) {
        constexpr float left   = 0.0f;
        const float right  = viewportWidth;
        constexpr float bottom = 0.0f;
        const float top    = viewportHeight;

        const float xCenter = (left + right) / 2.0f - mesh->getWidth() * 0.5f;
        const float yCenter = (bottom + top) / 2.0f - mesh->getSizeY() * 0.5f;

        align = glm::vec2(xCenter, yCenter);
    }

    float LabelNode2D::getWidth() const {
        return mesh ? mesh->getWidth() : 0.0f;
    }

    float LabelNode2D::getHeight() const {
        return mesh ? mesh->getSizeY() : 0.0f;
    }
} // namespace Model
