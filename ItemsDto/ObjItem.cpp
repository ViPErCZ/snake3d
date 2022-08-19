#include "ObjItem.h"
#include "../tiny_obj_loader.h"

namespace ItemsDto {

    bool ObjItem::load(const string &filename) {
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./"; // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filename, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        // Loop over shapes
        for (const auto & shape : shapes) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                auto fv = size_t(shape.mesh.num_face_vertices[f]);

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

                    glm::vec3 vertex = {vx, vy, vz};
                    vertices.push_back(vertex);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                        glm::vec3 normal = {nx, ny, nz};
                        normals.push_back(normal);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                        glm::vec2 uv = {tx, ty};
                        uvs.push_back(uv);
                    }

                    // Optional: vertex colors
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                }
                index_offset += fv;

                // per-face material
                //shape.mesh.material_ids[f];
            }
        }

        generateBuffer();

        cout << "Loading OBJ file " << filename << " success" << endl;

        return true;
    }

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
} // ItemsDto