#ifndef SNAKE3_MESH2D_H
#define SNAKE3_MESH2D_H

#include <snake3d/Renderer/Opengl/Model/Utils/Vao.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Ebo.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace ModelUtils {
    class Mesh2D {
    public:
        Mesh2D(const std::vector<Vertex2D> &vertices, const std::vector<unsigned int> &indices, std::string name = "");

        virtual ~Mesh2D();

        [[nodiscard]] const std::vector<GLuint> &getIndices() const;
        [[nodiscard]] const std::vector<Vertex2D> &getVertices() const;
        [[nodiscard]] const glm::mat4 &getGlobalTransformation() const;
        [[nodiscard]] const std::string &getName() const;
        void setGlobalTransformation(const glm::mat4 &globalTransformation);

        void initialize();

        void bind();

    protected:
        std::vector<Vertex2D> vertices;
        std::vector<unsigned int> indices;
        Vao *vao;
        glm::mat4 globalTransformation{};
        std::string name;
    };
} // ModelUtils

#endif //SNAKE3_MESH2D_H