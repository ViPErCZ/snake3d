#include "LabelNode2D.h"
#include "../../../../../Manager/VboIndexer.h"

namespace Model {
    LabelNode2D::LabelNode2D(const std::string &text, const shared_ptr<ShaderManager> &baseShader,
                             const shared_ptr<LabelSettings> &settings) : BaseNode2D(baseShader) {
        if (!settings->getFont()) {
            std::cerr << "TextMesh: Font is null!" << std::endl;
            return;
        }

        const auto &font = settings->getFont();

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        float x = 0.0f;
        float y = 0.0f;
        GLuint indexOffset = 0;

        auto addQuad = [&](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
                           glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
            vertices.push_back({p0, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, uv0});
            vertices.push_back({p1, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, uv1});
            vertices.push_back({p2, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, uv2});
            vertices.push_back({p3, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, uv3});

            indices.insert(indices.end(), {
                               indexOffset, indexOffset + 1, indexOffset + 2,
                               indexOffset, indexOffset + 2, indexOffset + 3
                           });
            indexOffset += 4;
        };

        for (char c: text) {
            const Character *ch = font->getCharacter(c);
            if (!ch) continue;

            float xpos = x + static_cast<float>(ch->bearing.x);
            float ypos = y - static_cast<float>((ch->size.y - ch->bearing.y));

            auto w = static_cast<float>(ch->size.x);
            auto h = static_cast<float>(ch->size.y);

            // UV v atlasu
            glm::vec2 uv0 = ch->uvOffset;
            glm::vec2 uv1 = ch->uvOffset + glm::vec2(ch->uvSize.x, 0.0f);
            glm::vec2 uv2 = ch->uvOffset + glm::vec2(ch->uvSize.x, ch->uvSize.y);
            glm::vec2 uv3 = ch->uvOffset + glm::vec2(0.0f, ch->uvSize.y);

            glm::vec3 p0(xpos, ypos + h, 0.0f);
            glm::vec3 p1(xpos + w, ypos + h, 0.0f);
            glm::vec3 p2(xpos + w, ypos, 0.0f);
            glm::vec3 p3(xpos, ypos, 0.0f);

            addQuad(p0, p1, p2, p3, uv0, uv1, uv2, uv3);

            // posun na další znak
            x += static_cast<float>(ch->advance >> 6) + settings->getLetterSpacing();
        }

        // --- Tangenty a biTangenty ---
        std::vector<glm::vec3> positions, normals;
        std::vector<glm::vec2> uvs;

        for (const auto &v: vertices) {
            positions.push_back(v.position);
            normals.push_back(v.normal);
            uvs.push_back(v.texUV);
        }

        std::vector<glm::vec3> tangents, biTangents;
        VboIndexer::computeTangentBasis(positions, uvs, normals, tangents, biTangents);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangents = tangents[i];
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices);
        textureId = font->getAtlasTextureId();
    }
} // Model
