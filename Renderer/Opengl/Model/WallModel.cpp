#include "WallModel.h"

namespace Model {
    WallModel::WallModel(ObjWall *wall) : wall(wall) {
        vector<Vertex> vertices;
        auto item = (*wall->getItems().begin());
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
        vao = new Vao();
        mesh = new Mesh(vertices, (*item).getIndices(), *vao, &(*item));
    }

    WallModel::~WallModel() {
        delete vao;
        delete mesh;
    }

    Mesh *WallModel::getMesh() const {
        return mesh;
    }

    Vao *WallModel::getVao() const {
        return vao;
    }

} // Model