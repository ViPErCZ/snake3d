#ifndef SNAKE3_MESH_H
#define SNAKE3_MESH_H

#include "Vao.h"
#include "Ebo.h"
#include "../../../../ItemsDto/BaseItem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace ItemsDto;
using namespace std;

namespace ModelUtils {

    class Mesh {
    public:
        Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, bool hasBones = false,
             string name = "");

        virtual ~Mesh();

        [[nodiscard]] const vector<GLuint> &getIndices() const;
        [[nodiscard]] const vector<Vertex> &getVertices() const;
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
        Vao *vao;
        bool hasBones;
        glm::mat4 globalTransformation{};
        string name;
        glm::vec3 localMin;
        glm::vec3 localMax;
    };

} // ModelUtils

#endif //SNAKE3_MESH_H
