#include "MeshModel.h"
#include "../../../Manager/VboIndexer.h"

namespace Model {
    MeshModel::MeshModel(BaseItem *baseItem) : item(baseItem) {
        vector<glm::vec3> vbo_vertices;
        vector<glm::vec3> vbo_normals;
        vector<glm::vec2> vbo_uvs;
        vector<glm::vec3> tangents;
        vector<glm::vec3> biTangents;

        vector<Vertex> vertices;
        vector<GLuint> indices;

        Vertex vertex{};
        vertex.position = {-1.0f, -1.0f, -1.0f};
        vertex.normal = {0.0f, 1.0f, 0.0f};
        vertex.color = {0.0f, 0.0f, 0.0f};
        vertex.texUV = {0.0f, 0.0f};

        vbo_vertices.push_back(vertex.position);
        vbo_uvs.push_back(vertex.texUV);
        vbo_normals.push_back(vertex.normal);

        vertices.push_back(vertex);
        indices.push_back(0);

        Vertex vertex2{};
        vertex2.position = {1.0f, -1.0f, -1.0f};
        vertex2.normal = {0.0f, 1.0f, 0.0f};
        vertex2.color = {0.0f, 0.0f, 0.0f};
        vertex2.texUV = {1.0f, 0.0f};

        vbo_vertices.push_back(vertex2.position);
        vbo_uvs.push_back(vertex2.texUV);
        vbo_normals.push_back(vertex2.normal);

        vertices.push_back(vertex2);
        indices.push_back(1);

        Vertex vertex3{};
        vertex3.position = {1.0f, 1.0f, -1.0f};
        vertex3.normal = {0.0f, 1.0f, 0.0f};
        vertex3.color = {0.0f, 0.0f, 0.0f};
        vertex3.texUV = {1.0f, 1.0f};

        vbo_vertices.push_back(vertex3.position);
        vbo_uvs.push_back(vertex3.texUV);
        vbo_normals.push_back(vertex3.normal);

        vertices.push_back(vertex3);
        indices.push_back(2);

        Vertex vertex4{};
        vertex4.position = {1.0f, 1.0f, -1.0f};
        vertex4.normal = {0.0f, 1.0f, 0.0f};
        vertex4.color = {0.0f, 0.0f, 0.0f};
        vertex4.texUV = {1.0f, 1.0f};

        vbo_vertices.push_back(vertex4.position);
        vbo_uvs.push_back(vertex4.texUV);
        vbo_normals.push_back(vertex4.normal);

        vertices.push_back(vertex4);
        indices.push_back(3);

        Vertex vertex5{};
        vertex5.position = {-1.0f, 1.0f, -1.0f};
        vertex5.normal = {0.0f, 1.0f, 0.0f};
        vertex5.color = {0.0f, 0.0f, 0.0f};
        vertex5.texUV = {0.0f, 1.0f};

        vbo_vertices.push_back(vertex5.position);
        vbo_uvs.push_back(vertex5.texUV);
        vbo_normals.push_back(vertex5.normal);

        vertices.push_back(vertex5);
        indices.push_back(4);

        Vertex vertex6{};
        vertex6.position = {-1.0f, -1.0f, -1.0f};
        vertex6.normal = {0.0f, 1.0f, 0.0f};
        vertex6.color = {0.0f, 0.0f, 0.0f};
        vertex6.texUV = {0.0f, 0.0f};

        vbo_vertices.push_back(vertex6.position);
        vbo_uvs.push_back(vertex6.texUV);
        vbo_normals.push_back(vertex6.normal);

        vertices.push_back(vertex6);
        indices.push_back(5);

        VboIndexer::computeTangentBasis(vbo_vertices, vbo_uvs, vbo_normals, tangents, biTangents);

        int index = 0;
        for (auto iter: tangents) {
            auto vertIter = vertices.begin() + index;
            (*vertIter).tangents = iter;
            index++;
        }

        mesh = new Mesh(vertices, indices);
    }

    MeshModel::~MeshModel() {
        delete mesh;
    }

    Mesh *MeshModel::getMesh() const {
        return mesh;
    }

} // Model