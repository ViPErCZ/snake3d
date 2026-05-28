#include <snake3d/Renderer/Opengl/Model/Utils/TextMesh.h>

using namespace std;
using namespace Material;

namespace ModelUtils {
    TextMesh::TextMesh() {
        vao = nullptr;
        vbo = nullptr;
    }

    void TextMesh::initialize() {
        if (vao != nullptr) {
            return;
        }

        vao = make_shared<Vao>();
        vbo = make_shared<Vbo>();
    }

    void TextMesh::update(const std::string &text, const shared_ptr<Font> &font) {
        if (vao == nullptr) {
            initialize();
        }

        vertices.clear();
        float x = 0.0f;
        const float y = font->getAscenderPixels();

        for (const char c: text) {
            const auto ch = font->getCharacter(c);

            float xpos = x + static_cast<float>(ch->bearing.x);
            const auto w = static_cast<float>(ch->size.x);
            const auto h = static_cast<float>(ch->size.y);

            float u0 = ch->uvOffset.x, v0 = ch->uvOffset.y;
            float u1 = u0 + ch->uvSize.x, v1 = v0 + ch->uvSize.y;

            const auto bearingY = static_cast<float>(ch->bearing.y);
            const float sizeY = h;
            const bool exclude = c == '-' || c == '`' || c == '~' || c == '\'' || c == '=';
            const float air_below = (bearingY > sizeY && !exclude) ? (bearingY - sizeY) : 0.0f;
            float ypos = y - bearingY + air_below;

            vertices.insert(vertices.end(), {
                        xpos, ypos + h, u0, v1, // Top-Left
                        xpos, ypos,     u0, v0, // Bottom-Left
                        xpos + w, ypos, u1, v0, // Bottom-Right

                        xpos, ypos + h, u0, v1, // Top-Left
                        xpos + w, ypos, u1, v0, // Bottom-Right
                        xpos + w, ypos + h, u1, v1  // Top-Right
                    });

            x += static_cast<float>(ch->advance);
        }

        width = x;
        maxSizeY = 0.8f * (font->getAscenderPx() - font->getDescenderPx());

        vao->bind();
        vbo->bind();
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4,GL_FLOAT,GL_FALSE, 4 * sizeof(float), static_cast<void *>(nullptr));
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(),
                     GL_DYNAMIC_DRAW);
    }

    void TextMesh::bind() const {
        vao->bind();
    }

    std::vector<float> TextMesh::getVertices() const {
        return vertices;
    }

    float TextMesh::getSizeY() const {
        return maxSizeY;
    }

    float TextMesh::getWidth() const {
        return width;
    }
} // ModelUtils
