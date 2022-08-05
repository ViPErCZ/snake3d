#include "ObjItem.h"
#include "../tiny_obj_loader.h"

namespace ItemsDto {
    ObjItem::~ObjItem() {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &uvBuffer);
        glDeleteBuffers(1, &normalBuffer);
    }

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
        Manager::VboIndexer::indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, (GLsizei)(indexed_vertices.size() * sizeof(glm::vec3)), &indexed_vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, (GLsizei)(indexed_uvs.size() * sizeof(glm::vec2)), &indexed_uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &normalBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, (GLsizei)(indexed_normals.size() * sizeof(glm::vec3)), &indexed_normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)(indices.size() * sizeof(unsigned short)), &indices[0], GL_STATIC_DRAW);
    }

    GLuint ObjItem::getVertexBuffer() const {
        return vertexBuffer;
    }

    GLuint ObjItem::getUvBuffer() const {
        return uvBuffer;
    }

    GLuint ObjItem::getNormalBuffer() const {
        return normalBuffer;
    }

    const vector<unsigned short> &ObjItem::getIndices() const {
        return indices;
    }

    GLuint ObjItem::getElementBuffer() const {
        return elementBuffer;
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
} // ItemsDto