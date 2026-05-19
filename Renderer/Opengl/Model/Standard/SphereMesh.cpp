#include "../../../../Manager/VboIndexer.h"
#include "SphereMesh.h"

#include <utility>

namespace Model {
    SphereMesh::SphereMesh(shared_ptr<ShaderProgram> baseShader, float height,
                           float radius, int rings, int segments) : StandardMesh(std::move(baseShader)) {
        float halfHeight = height * 0.5f;
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        auto addVertex = [&](const glm::vec3 pos, const glm::vec3 normal, const glm::vec2 uv, const glm::vec3 color = {1.0f, 1.0f, 1.0f}) {
            Vertex v{};
            v.position = pos;
            v.normal = normal;
            v.color = color;
            v.texUV = uv;
            vertices.push_back(v);
        };

        for (int y = 0; y <= rings; ++y) {
            float v = static_cast<float>(y) / static_cast<float>(rings); // 0..1
            float theta = v * glm::pi<float>(); // 0..pi

            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (int x = 0; x <= segments; ++x) {
                float u = static_cast<float>(x) / static_cast<float>(segments); // 0..1
                float phi = u * glm::two_pi<float>(); // 0..2pi

                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                auto pos = glm::vec3(
                    radius * sinTheta * cosPhi, // X
                    halfHeight * cosTheta, // Y
                    radius * sinTheta * sinPhi // Z
                );

                glm::vec3 normal = glm::normalize(glm::vec3(pos.x / radius, pos.y / halfHeight, pos.z / radius));

                addVertex(pos, normal, {u, v});
            }
        }

        for (int y = 0; y < rings; ++y) {
            for (int x = 0; x < segments; ++x) {
                GLuint i0 = y * (segments + 1) + x;
                GLuint i1 = i0 + 1;
                GLuint i2 = i0 + (segments + 1);
                GLuint i3 = i2 + 1;

                indices.push_back(i0);
                indices.push_back(i2);
                indices.push_back(i1);

                indices.push_back(i1);
                indices.push_back(i2);
                indices.push_back(i3);
            }
        }

        std::vector<glm::vec3> positions, normalsVec;
        std::vector<glm::vec2> uvsVec;
        positions.reserve(vertices.size());
        normalsVec.reserve(vertices.size());
        uvsVec.reserve(vertices.size());

        for (const auto &v: vertices) {
            positions.push_back(v.position);
            normalsVec.push_back(v.normal);
            uvsVec.push_back(v.texUV);
            localMin = glm::min(localMin, v.position);
            localMax = glm::max(localMax, v.position);
        }

        std::vector<glm::vec3> tangents, biTangents;
        VboIndexer::computeTangentBasis(positions, uvsVec, normalsVec, tangents, biTangents);

        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangents = tangents[i];
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices);
    }
} // Model
