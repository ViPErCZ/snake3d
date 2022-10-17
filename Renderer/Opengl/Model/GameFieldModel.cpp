#include "GameFieldModel.h"
#include "../../../Manager/VboIndexer.h"

namespace Model {
    GameFieldModel::GameFieldModel(BaseItem *baseItem) : gameField(baseItem) {
        vector<glm::vec3> vbo_vertices;
        vector<glm::vec3> vbo_normals;
        vector<glm::vec2> vbo_uvs;
        vector<glm::vec3> tangents;
        vector<glm::vec3> biTangents;

        vector<Vertex> vertices;
        vector<GLuint> indices;

        Vertex vertex{};
        vertex.position = {-1.0f, -1.0f, -1.0f};
        vertex.normal = glm::normalize(vertex.position);
        vertex.color = {1.0f, 1.0f, 1.0f};
        vertex.texUV = {0.0f, 0.0f};

        vbo_vertices.push_back(vertex.position);
        vbo_uvs.push_back(vertex.texUV);
        vbo_normals.push_back(vertex.normal);

        vertices.push_back(vertex);
        indices.push_back(0);

        Vertex vertex2{};
        vertex2.position = {3.0f, -1.0f, -1.0f};
        vertex2.normal = glm::normalize(vertex2.position);
        vertex2.color = {1.0f, 1.0f, 1.0f};
        vertex2.texUV = {2.0f, 0.0f};

        vbo_vertices.push_back(vertex2.position);
        vbo_uvs.push_back(vertex2.texUV);
        vbo_normals.push_back(vertex2.normal);

        vertices.push_back(vertex2);
        indices.push_back(1);

        Vertex vertex3{};
        vertex3.position = {3.0f, 3.0f, -1.0f};
        vertex3.normal = glm::normalize(vertex3.position);
        vertex3.color = {1.0f, 1.0f, 1.0f};
        vertex3.texUV = {2.0f, 2.0f};

        vbo_vertices.push_back(vertex3.position);
        vbo_uvs.push_back(vertex3.texUV);
        vbo_normals.push_back(vertex3.normal);

        vertices.push_back(vertex3);
        indices.push_back(2);

        Vertex vertex4{};
        vertex4.position = {3.0f, 3.0f, -1.0f};
        vertex4.normal = glm::normalize(vertex4.position);
        vertex4.color = {1.0f, 1.0f, 1.0f};
        vertex4.texUV = {2.0f, 2.0f};

        vbo_vertices.push_back(vertex4.position);
        vbo_uvs.push_back(vertex4.texUV);
        vbo_normals.push_back(vertex4.normal);

        vertices.push_back(vertex4);
        indices.push_back(3);

        Vertex vertex5{};
        vertex5.position = {-1.0f, 3.0f, -1.0f};
        vertex5.normal = glm::normalize(vertex5.position);
        vertex5.color = {1.0f, 1.0f, 1.0f};
        vertex5.texUV = {0.0f, 2.0f};

        vbo_vertices.push_back(vertex5.position);
        vbo_uvs.push_back(vertex5.texUV);
        vbo_normals.push_back(vertex5.normal);

        vertices.push_back(vertex5);
        indices.push_back(4);

        Vertex vertex6{};
        vertex6.position = {-1.0f, -1.0f, -1.0f};
        vertex6.normal = glm::normalize(vertex6.position);
        vertex6.color = {1.0f, 1.0f, 1.0f};
        vertex6.texUV = {0.0f, 0.0f};

        vbo_vertices.push_back(vertex6.position);
        vbo_uvs.push_back(vertex6.texUV);
        vbo_normals.push_back(vertex6.normal);

        vertices.push_back(vertex6);
        indices.push_back(5);

        Manager::VboIndexer::computeTangentBasis(vbo_vertices, vbo_uvs, vbo_normals, tangents, biTangents);

        int index = 0;
        for (auto iter : tangents) {
            auto vertIter = vertices.begin() + index;
            (*vertIter).tangents = iter;
            index++;
        }

        mesh = new Mesh(vertices, indices, vao, gameField);
    }

    GameFieldModel::~GameFieldModel() {
        delete mesh;
    }

    Mesh *GameFieldModel::getMesh() const {
        return mesh;
    }

    const Vao &GameFieldModel::getVao() const {
        return vao;
    }

    BaseItem *GameFieldModel::getGameField() const {
        return gameField;
    }

} // Model