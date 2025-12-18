#include <iostream>

#include "ObjModelLoader.h"

#include "../Manager/VboIndexer.h"
#include "../Thirdparty/tinyobj/tiny_obj_loader.h"

namespace Resource {
    shared_ptr<Mesh> ObjModelLoader::loadObj(const fs::path &path) {
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./"; // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(path, reader_config)) {
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
        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;

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

        std::vector<unsigned int> indices;
        vector<glm::vec3> tangents;
        vector<glm::vec3> biTangents;
        std::vector<glm::vec3> indexed_vertices;
        std::vector<glm::vec2> indexed_uvs;
        std::vector<glm::vec3> indexed_normals;
        std::vector<glm::vec3> indexed_tangents;
        std::vector<glm::vec3> indexed_biTangents;
        Manager::VboIndexer::computeTangentBasis(vertices, uvs, normals, tangents, biTangents);
        Manager::VboIndexer::indexVBO_TBN(vertices, uvs, normals, tangents, biTangents,
                                          indices, indexed_vertices,
                                          indexed_uvs, indexed_normals, indexed_tangents, indexed_biTangents);

        vector<Vertex> vertex_vertices;
        int index = 0;
        for (auto vert: indexed_vertices) {
            Vertex vertex{};
            vertex.position = vert;
            vertex.normal = *(indexed_normals.begin() + index);
            vertex.color = {1.0f, 1.0f, 1.0f};
            vertex.texUV = *(indexed_uvs.begin() + index);
            vertex.tangents = *(indexed_tangents.begin() + index);
            vertex.biTangents = *(indexed_biTangents.begin() + index);

            vertex_vertices.push_back(vertex);
            index++;
        }

        return std::make_shared<Mesh>(vertex_vertices, indices);
    }

    shared_ptr<Mesh> ObjModelLoader::loadObjFromStr(const fs::path &path, const string &str) {
        std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        tinyobj::ObjReaderConfig reader_config;
        tinyobj::ObjReader reader;

        if (!reader.ParseFromString(path, str, reader_config)) {
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
        vector<glm::vec3> vertices;
        vector<glm::vec2> uvs;
        vector<glm::vec3> normals;

        // Loop over shapes
        for (const auto & shape : shapes) {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                auto fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);

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

        std::vector<unsigned int> indices;
        vector<glm::vec3> tangents;
        vector<glm::vec3> biTangents;
        std::vector<glm::vec3> indexed_vertices;
        std::vector<glm::vec2> indexed_uvs;
        std::vector<glm::vec3> indexed_normals;
        std::vector<glm::vec3> indexed_tangents;
        std::vector<glm::vec3> indexed_biTangents;
        Manager::VboIndexer::computeTangentBasis(vertices, uvs, normals, tangents, biTangents);
        Manager::VboIndexer::indexVBO_TBN(vertices, uvs, normals, tangents, biTangents,
                                          indices, indexed_vertices,
                                          indexed_uvs, indexed_normals, indexed_tangents, indexed_biTangents);

        vector<Vertex> vertex_vertices;
        int index = 0;
        for (auto vert: indexed_vertices) {
            Vertex vertex{};
            vertex.position = vert;
            vertex.normal = *(indexed_normals.begin() + index);
            vertex.color = {1.0f, 1.0f, 1.0f};
            vertex.texUV = *(indexed_uvs.begin() + index);
            vertex.tangents = *(indexed_tangents.begin() + index);
            vertex.biTangents = *(indexed_biTangents.begin() + index);

            vertex_vertices.push_back(vertex);
            index++;
        }

        return std::make_shared<Mesh>(vertex_vertices, indices);
    }
} // Resource