#ifndef SNAKE3_MESH_H
#define SNAKE3_MESH_H

#include <snake3d/Renderer/Opengl/Model/Utils/Vao.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Ebo.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace ModelUtils {

    class Mesh {
    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, bool hasBones = false,
             std::string name = "");

        Mesh(const std::vector<Vertex> &ve, const std::vector<unsigned int> &i, const std::vector<TextureInfo> &t)
        : vertices(ve), indices(i), textures(t) {
            hasBones = false; name = ""; vao = nullptr;
            for (const auto &v: vertices) {
                localMin = glm::min(localMin, v.position);
                localMax = glm::max(localMax, v.position);
            }
        }

        virtual ~Mesh();

        [[nodiscard]] const std::vector<GLuint> &getIndices() const;
        [[nodiscard]] const std::vector<Vertex> &getVertices() const;
        [[nodiscard]] const std::vector<TextureInfo> &getTextures() const;
        [[nodiscard]] const glm::mat4 &getGlobalTransformation() const;
        [[nodiscard]] bool isHasBones() const;
        [[nodiscard]] const std::string &getName() const;
        void setGlobalTransformation(const glm::mat4 &globalTransformation);

        void initialize();

        void bind();

        [[nodiscard]] glm::vec3 getMin(const glm::mat4 &modelMatrix) const;

        [[nodiscard]] glm::vec3 getMax(const glm::mat4 &modelMatrix) const;

    protected:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureInfo> textures;
        Vao *vao;
        bool hasBones;
        glm::mat4 globalTransformation{};
        std::string name;
        glm::vec3 localMin{};
        glm::vec3 localMax{};
    };

} // ModelUtils

#endif //SNAKE3_MESH_H
