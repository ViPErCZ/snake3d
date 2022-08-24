#include "EatModel.h"

namespace Model {
    EatModel::EatModel(Eat *eat) : eat(eat) {
        vector<Vertex> vertices;
        int index = 0;
        for (auto vert: eat->getIndexedVertices()) {
            auto uvs = eat->getIndexedUvs().begin() + index;
            auto normals = eat->getIndexedNormals().begin() + index;
            auto tangents = eat->getIndexedTangents().begin() + index;
            auto biTangents = eat->getIndexedBiTangents().begin() + index;
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
        meshes.insert(pair<Vao*, Mesh*>(vao, new Mesh(vertices, eat->getIndices(), *vao, eat)));
    }

    EatModel::~EatModel() {
        for (auto data: meshes) {
            delete data.first;
            delete data.second;
        }

        meshes.clear();
    }

    const map<Vao *, Mesh *> &EatModel::getMeshes() const {
        return meshes;
    }
} // Model