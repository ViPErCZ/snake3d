#include "../../../../Manager/VboIndexer.h"
#include "BoxMesh.h"

#include <utility>

namespace Model {
    BoxMesh::BoxMesh(shared_ptr<ShaderManager> baseShader,
                     const float width, const float height, const float depth)
        : StandardMesh(std::move(baseShader)), depth(depth) {
        float hw = width * 0.5f;
        float hh = height * 0.5f;
        float hd = depth * 0.5f;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        auto addVertex = [&](glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, glm::vec3 color = {1.0f, 1.0f, 1.0f}) {
            Vertex v{};
            v.position = pos;
            v.normal = normal;
            v.color = color;
            v.texUV = uv;
            vertices.push_back(v);
        };

        auto addFace = [&](const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 v3,
                           const glm::vec3 normal) {
            const auto base = static_cast<GLuint>(vertices.size());
            addVertex(v0, normal, {0.0f, 0.0f});
            addVertex(v1, normal, {1.0f, 0.0f});
            addVertex(v2, normal, {1.0f, 1.0f});
            addVertex(v3, normal, {0.0f, 1.0f});

            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);

            indices.push_back(base + 2);
            indices.push_back(base + 3);
            indices.push_back(base + 0);
        };

        // +X
        addFace({hw, -hh, -hd}, {hw, -hh, hd}, {hw, hh, hd}, {hw, hh, -hd}, {1, 0, 0});
        // -X
        addFace({-hw, -hh, hd}, {-hw, -hh, -hd}, {-hw, hh, -hd}, {-hw, hh, hd}, {-1, 0, 0});
        // +Y
        addFace({-hw, hh, -hd}, {hw, hh, -hd}, {hw, hh, hd}, {-hw, hh, hd}, {0, 1, 0});
        // -Y
        addFace({-hw, -hh, hd}, {hw, -hh, hd}, {hw, -hh, -hd}, {-hw, -hh, -hd}, {0, -1, 0});
        // +Z
        addFace({hw, -hh, hd}, {-hw, -hh, hd}, {-hw, hh, hd}, {hw, hh, hd}, {0, 0, 1});
        // -Z
        addFace({-hw, -hh, -hd}, {hw, -hh, -hd}, {hw, hh, -hd}, {-hw, hh, -hd}, {0, 0, -1});

        std::vector<glm::vec3> positions, normals;
        std::vector<glm::vec2> uvs;
        positions.reserve(vertices.size());
        normals.reserve(vertices.size());
        uvs.reserve(vertices.size());

        for (const auto &v: vertices) {
            positions.push_back(v.position);
            normals.push_back(v.normal);
            uvs.push_back(v.texUV);
            localMin = glm::min(localMin, v.position);
            localMax = glm::max(localMax, v.position);
        }

        std::vector<glm::vec3> tangents, biTangents;
        VboIndexer::computeTangentBasis(positions, uvs, normals, tangents, biTangents);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangents = tangents[i];
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model
