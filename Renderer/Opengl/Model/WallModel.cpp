#include "WallModel.h"

namespace Model {
    WallModel::WallModel(ObjWall *wall) : wall(wall) {
        vector<Vertex> vertices;
        for (auto item : wall->getItems()) {
            int index = 0;
            for (auto vert: (*item).getIndexedVertices()) {
                auto uvs = (*item).getIndexedUvs().begin() + index;
                auto normals = (*item).getIndexedNormals().begin() + index;
                auto tangents = (*item).getIndexedTangents().begin() + index;
                auto biTangents = (*item).getIndexedBiTangents().begin() + index;
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
            meshes.insert(pair<Vao*, Mesh*>(vao, new Mesh(vertices, (*item).getIndices(), *vao, &(*item))));
        }
    }

    const map<Vao *, Mesh *> &WallModel::getMeshes() const {
        return meshes;
    }

    WallModel::~WallModel() {
        for (auto data: meshes) {
            delete data.first;
            delete data.second;
        }

        meshes.clear();
    }

} // Model