#include "../../../../Manager/VboIndexer.h"
#include "PlaneMesh.h"

#include <utility>


namespace Model {
    PlaneMesh::PlaneMesh(std::shared_ptr<BaseItem> baseItem, shared_ptr<ShaderManager> baseShader, const float width,
                         const float height) : StandardMesh(
        std::move(baseItem), std::move(baseShader), width, height) {
        float hw = width * 0.5f;
        float hd = height * 0.5f;

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        auto addVertex = [&](glm::vec3 pos, glm::vec2 uv, glm::vec3 color = {1.0f, 1.0f, 1.0f}) {
            Vertex v{};
            v.position = pos;
            v.normal = {0.0f, 1.0f, 0.0f};
            v.color = color;
            v.texUV = uv;
            vertices.push_back(v);
        };

        addVertex({-hw, 0.0f, -hd}, {0.0f, 0.0f}); // 0
        addVertex({hw, 0.0f, -hd}, {1.0f, 0.0f}); // 1
        addVertex({hw, 0.0f, hd}, {1.0f, 1.0f}); // 2

        addVertex({-hw, 0.0f, -hd}, {0.0f, 0.0f}); // 3
        addVertex({hw, 0.0f, hd}, {1.0f, 1.0f}); // 4
        addVertex({-hw, 0.0f, hd}, {0.0f, 1.0f}); // 5

        indices.reserve(6);
        for (GLuint i = 0; i < 6; ++i) {
            indices.push_back(i);
        }

        std::vector<glm::vec3> positions, normals;
        std::vector<glm::vec2> uvs;

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
        }
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model
