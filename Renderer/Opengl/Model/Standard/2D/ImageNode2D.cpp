#include "ImageNode2D.h"

#include "../../../Material/ShaderMaterial.h"

namespace Model {
    ImageNode2D::ImageNode2D(const float width, const float height,
        const shared_ptr<ShaderProgram> &baseShader,
        const shared_ptr<TextureManager> &texture)
        : BaseNode2D(baseShader), texture(texture), size(width, height) {
        std::vector<Vertex2D> vertices(4);

        const float halfW = width / 2.0f;
        const float halfH = height / 2.0f;

        vertices[0].position = {-halfW, -halfH,  0.0f};
        vertices[1].position = { halfW, -halfH,  0.0f};
        vertices[2].position = { halfW,  halfH,  0.0f};
        vertices[3].position = {-halfW,  halfH,  0.0f};

        vertices[0].texUV = {0.0f, 0.0f};
        vertices[1].texUV = {1.0f, 0.0f};
        vertices[2].texUV = {1.0f, 1.0f};
        vertices[3].texUV = {0.0f, 1.0f};

        std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

        mesh = std::make_shared<Mesh2D>(vertices, indices);
    }

    void ImageNode2D::render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, const float dt,
                             const glm::mat4 &parentTransform) const {
        if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            shaderMaterial->bind(
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
            baseShader->setInt("textureMap", 0);
            baseShader->setVec4("tint", {1.0f, 1.0f, 1.0f, 1.0f});
            baseShader->setFloat("alpha", 1.0f);
        }

        if (texture) {
            texture->bind(0);
        }

        mesh->bind();
        glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT, nullptr);

        if (texture) {
            texture->unbind(0);
        }

        if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            shaderMaterial->unbind();
        }
    }

    const glm::vec2 &ImageNode2D::getSize() const {
        return size;
    }
} // Model
