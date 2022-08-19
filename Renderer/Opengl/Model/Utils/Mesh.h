#ifndef SNAKE3_MESH_H
#define SNAKE3_MESH_H

#include "Vao.h"
#include "Ebo.h"
#include "../../../../ItemsDto/BaseItem.h"
#include "../../../../Manager/Camera.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/ShaderManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace ItemsDto;
using namespace std;

namespace ModelUtils {

    class Mesh {
    public:
        Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indices, const Vao &vao, BaseItem *item);
        [[nodiscard]] const vector<GLuint> &getIndices() const;

    protected:
        vector <Vertex> vertices;
        vector <unsigned int> indices;
        Vao vao;
        BaseItem* item;
    };

} // ModelUtils

#endif //SNAKE3_MESH_H
