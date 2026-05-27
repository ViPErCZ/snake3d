#ifndef SNAKE3_TEXTMESH_H
#define SNAKE3_TEXTMESH_H

#include <string>
#include <memory>

#include "Vao.h"
#include "Ebo.h"
#include "../../Material/2D/Font.h"

namespace ModelUtils {
    class TextMesh {
    public:
        TextMesh();

        void initialize();

        void bind() const;

        void update(const std::string &text, const std::shared_ptr<Material::Font> &font);

        [[nodiscard]] std::vector<float> getVertices() const;

        [[nodiscard]] float getSizeY() const;

        [[nodiscard]] float getWidth() const;

    protected:
        std::shared_ptr<Vao> vao{};
        std::shared_ptr<Vbo> vbo{};
        std::vector<float> vertices;
        float maxSizeY = 0.0f;
        float width = 0.0f;
    };
} // ModelUtils

#endif //SNAKE3_TEXTMESH_H
