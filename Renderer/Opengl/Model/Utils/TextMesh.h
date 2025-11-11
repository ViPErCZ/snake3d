#ifndef SNAKE3_TEXTMESH_H
#define SNAKE3_TEXTMESH_H

#include <string>
#include <memory>

#include "Vao.h"
#include "Ebo.h"
#include "../../Material/2D/Font.h"

using namespace std;
using namespace Material;

namespace ModelUtils {
    class TextMesh {
    public:
        TextMesh();

        void initialize();

        void bind(const std::string &text, const shared_ptr<Font> &font);

        [[nodiscard]] std::vector<float> getVertices() const;

        [[nodiscard]] float getSizeY() const;

    protected:
        shared_ptr<Vao> vao{};
        shared_ptr<Vbo> vbo{};
        std::vector<float> vertices;
        float maxSizeY = 0.0f;
    };
} // ModelUtils

#endif //SNAKE3_TEXTMESH_H
