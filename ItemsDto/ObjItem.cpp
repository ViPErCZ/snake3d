#include "ObjItem.h"

namespace ItemsDto {

    ObjItem::ObjItem(const vector<glm::vec3> &vertices, const vector<glm::vec2> &uvs, const vector<glm::vec3> &normals)
            : vertices(vertices), uvs(uvs), normals(normals) {
        generateBuffer();
    }

    ObjItem::ObjItem() = default;

    const vector<glm::vec3> &ObjItem::getVertices() const {
        return vertices;
    }

    const vector<glm::vec2> &ObjItem::getUvs() const {
        return uvs;
    }

    const vector<glm::vec3> &ObjItem::getNormals() const {
        return normals;
    }

    void ObjItem::generateBuffer() {
        Manager::VboIndexer::computeTangentBasis(vertices, uvs, normals, tangents, biTangents);
        Manager::VboIndexer::indexVBO_TBN(vertices, uvs, normals, tangents, biTangents,
                                          indices, indexed_vertices,
                                          indexed_uvs, indexed_normals, indexed_tangents, indexed_biTangents);

        vector<Vertex> vertices;
        int index = 0;
        for (auto vert: indexed_vertices) {
            auto uvs = indexed_uvs.begin() + index;
            auto normals = indexed_normals.begin() + index;
            auto tangents = indexed_tangents.begin() + index;
            auto biTangents = indexed_biTangents.begin() + index;
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

        mesh = new Mesh(vertices, indices);
    }

    const vector<unsigned int> &ObjItem::getIndices() const {
        return indices;
    }

    const vector<glm::vec3> &ObjItem::getIndexedVertices() const {
        return indexed_vertices;
    }

    const vector<glm::vec2> &ObjItem::getIndexedUvs() const {
        return indexed_uvs;
    }

    const vector<glm::vec3> &ObjItem::getIndexedNormals() const {
        return indexed_normals;
    }

    const vector<glm::vec3> &ObjItem::getIndexedTangents() const {
        return indexed_tangents;
    }

    const vector<glm::vec3> &ObjItem::getIndexedBiTangents() const {
        return indexed_biTangents;
    }

    Mesh *ObjItem::getMesh() const {
        return mesh;
    }

    ObjItem::~ObjItem() {
        delete mesh;
    }

} // ItemsDto