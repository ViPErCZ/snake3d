#ifndef SNAKE3_MESH_H
#define SNAKE3_MESH_H

#include "Vao.h"
#include "Ebo.h"
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

namespace ModelUtils {

    class Mesh {
    public:
        Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, bool hasBones = false,
             string name = "");

        Mesh(const vector<Vertex> &ve, const vector<unsigned int> &i, const vector<TextureInfo> &t)
        : vertices(ve), indices(i), textures(t) {
            hasBones = false; name = ""; vao = nullptr;
            for (const auto &v: vertices) {
                localMin = glm::min(localMin, v.position);
                localMax = glm::max(localMax, v.position);
            }
        }

        virtual ~Mesh();

        [[nodiscard]] const vector<GLuint> &getIndices() const;
        [[nodiscard]] const vector<Vertex> &getVertices() const;
        [[nodiscard]] const vector<TextureInfo> &getTextures() const;
        [[nodiscard]] const glm::mat4 &getGlobalTransformation() const;
        [[nodiscard]] bool isHasBones() const;
        [[nodiscard]] const string &getName() const;
        void setGlobalTransformation(const glm::mat4 &globalTransformation);

        void initialize();

        void bind();

        [[nodiscard]] glm::vec3 getMin(const glm::mat4 &modelMatrix) const;

        [[nodiscard]] glm::vec3 getMax(const glm::mat4 &modelMatrix) const;

    protected:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        std::vector<TextureInfo> textures;
        Vao *vao;
        bool hasBones;
        glm::mat4 globalTransformation{};
        string name;
        glm::vec3 localMin{};
        glm::vec3 localMax{};
    };

} // ModelUtils

#endif //SNAKE3_MESH_H
