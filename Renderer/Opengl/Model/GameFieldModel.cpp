#include "GameFieldModel.h"

namespace Model {
    GameFieldModel::GameFieldModel(BaseItem *baseItem) : gameField(baseItem) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        Vertex vertex{};
        vertex.position = {-1.0f, -1.0f, -1.0f};
        vertex.normal = glm::normalize(vertex.position);
        vertex.color = {1.0f, 1.0f, 1.0f};
        vertex.texUV = {0.0f, 0.0f};

        vertices.push_back(vertex);
        indices.push_back(0);

        Vertex vertex2{};
        vertex2.position = {1.0f, -1.0f, -1.0f};
        vertex2.normal = glm::normalize(vertex2.position);
        vertex2.color = {1.0f, 1.0f, 1.0f};
        vertex2.texUV = {1.0f, 0.0f};

        vertices.push_back(vertex2);
        indices.push_back(1);

        Vertex vertex3{};
        vertex3.position = {1.0f, 1.0f, -1.0f};
        vertex3.normal = glm::normalize(vertex3.position);
        vertex3.color = {1.0f, 1.0f, 1.0f};
        vertex3.texUV = {1.0f, 1.0f};

        vertices.push_back(vertex3);
        indices.push_back(2);

        Vertex vertex4{};
        vertex4.position = {1.0f, 1.0f, -1.0f};
        vertex4.normal = glm::normalize(vertex4.position);
        vertex4.color = {1.0f, 1.0f, 1.0f};
        vertex4.texUV = {1.0f, 1.0f};

        vertices.push_back(vertex4);
        indices.push_back(3);

        Vertex vertex5{};
        vertex5.position = {-1.0f, 1.0f, -1.0f};
        vertex5.normal = glm::normalize(vertex5.position);
        vertex5.color = {1.0f, 1.0f, 1.0f};
        vertex5.texUV = {0.0f, 1.0f};

        vertices.push_back(vertex5);
        indices.push_back(4);

        Vertex vertex6{};
        vertex6.position = {-1.0f, -1.0f, -1.0f};
        vertex6.normal = glm::normalize(vertex6.position);
        vertex6.color = {1.0f, 1.0f, 1.0f};
        vertex6.texUV = {0.0f, 0.0f};

        vertices.push_back(vertex6);
        indices.push_back(5);

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