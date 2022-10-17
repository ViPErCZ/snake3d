#include "SnakeModel.h"

namespace Model {
    SnakeModel::SnakeModel(Snake *snake) : snake(snake) {
        for (auto item : snake->getItems()) {
            generateMeshes(item);
        }
    }

    void SnakeModel::generateMeshes(const sSNAKE_TILE *item) {
        vector<Vertex> vertices;
        int index = 0;
        for (auto vert: (*item).tile->getIndexedVertices()) {
            auto uvs = (*item).tile->getIndexedUvs().begin() + index;
            auto normals = (*item).tile->getIndexedNormals().begin() + index;
            auto tangents = (*item).tile->getIndexedTangents().begin() + index;
            auto biTangents = (*item).tile->getIndexedBiTangents().begin() + index;
            Vertex vertex{};
            vertex.position = vert;
            vertex.normal = (*normals);
            vertex.color = {1.0f, 1.0f, 1.0f};
            vertex.texUV = (*uvs);
            vertex.tangents = (*tangents);
            vertex.biTangents = (*biTangents);

            vertices.push_back(vertex);
            index++;
        }

        auto vao = new Vao();
        ObjItem* tile = (*item).tile;
        meshes.insert(pair<Vao*, Mesh*>(vao, new Mesh(vertices, (*item).tile->getIndices(), *vao, tile)));
    }

    SnakeModel::~SnakeModel() {
        for (auto data: meshes) {
            delete data.first;
            delete data.second;
        }

        meshes.clear();
    }

    const map<Vao *, Mesh *> &SnakeModel::getMeshes() const {
        return meshes;
    }

    void SnakeModel::addTiles(int tilesCount) {
        int counter = 0;
        for (auto Iter = snake->getItems().end() - 1; Iter >= snake->getItems().begin(); Iter--) {
            if (counter == tilesCount) {
                break;
            }
            generateMeshes((*Iter));
            counter++;
        }
    }
} // Model