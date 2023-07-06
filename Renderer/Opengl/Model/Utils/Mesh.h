#ifndef SNAKE3_MESH_H
#define SNAKE3_MESH_H

#include "Vao.h"
#include "Ebo.h"
#include "../../../../ItemsDto/BaseItem.h"
#include "../../../../Manager/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <cstddef>

using namespace ItemsDto;
using namespace std;

namespace ModelUtils {

    class Mesh {
    public:
        Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, bool hasBones = false,
             string name = "");

        virtual ~Mesh();

        [[nodiscard]] const vector<GLuint> &getIndices() const;
        [[nodiscard]] const glm::mat4 &getGlobalTransformation() const;
        [[nodiscard]] bool isHasBones() const;
        [[nodiscard]] const string &getName() const;
        void setGlobalTransformation(const glm::mat4 &globalTransformation);

        void bind();

    protected:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        Vao *vao;
        bool hasBones;
        glm::mat4 globalTransformation{};
        string name;
    };

} // ModelUtils

#endif //SNAKE3_MESH_H
