#ifndef SNAKE3_MESH2D_H
#define SNAKE3_MESH2D_H

#include "Vao.h"
#include "Ebo.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace ModelUtils {
    class Mesh2D {
    public:
        Mesh2D(const vector<Vertex2D> &vertices, const vector<unsigned int> &indices, string name = "");

        virtual ~Mesh2D();

        [[nodiscard]] const vector<GLuint> &getIndices() const;
        [[nodiscard]] const vector<Vertex2D> &getVertices() const;
        [[nodiscard]] const glm::mat4 &getGlobalTransformation() const;
        [[nodiscard]] const string &getName() const;
        void setGlobalTransformation(const glm::mat4 &globalTransformation);

        void initialize();

        void bind();

    protected:
        vector<Vertex2D> vertices;
        vector<unsigned int> indices;
        Vao *vao;
        glm::mat4 globalTransformation{};
        string name;
    };
} // ModelUtils

#endif //SNAKE3_MESH2D_H