#include "ArrayMesh.h"
#include <snake3d/Manager/VboIndexer.h>

using namespace Manager;
using namespace ModelUtils;
using namespace std;

namespace Model {
    void ArrayMesh::fromMesh(const shared_ptr<Mesh> &mesh) {
        this->mesh = mesh;
        computeLocalAABB();
    }

    void ArrayMesh::fromVertexData(vector<Vertex> &vertices) {
        std::vector<GLuint> indices;

        indices.reserve(6);
        for (GLuint i = 0; i < 6; ++i) {
            indices.push_back(i);
        }

        std::vector<glm::vec3> positions, normals;
        std::vector<glm::vec2> uvs;

        for (const auto &v: vertices) {
            positions.push_back(v.position);
            normals.push_back(v.normal);
            uvs.push_back(v.texUV);
            localMin = glm::min(localMin, v.position);
            localMax = glm::max(localMax, v.position);
        }

        std::vector<glm::vec3> tangents, biTangents;
        VboIndexer::computeTangentBasis(positions, uvs, normals, tangents, biTangents);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangents = tangents[i];
        }
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model
