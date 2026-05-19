#include "../../../../Manager/VboIndexer.h"
#include "CylinderMesh.h"

namespace Model {
    CylinderMesh::CylinderMesh(std::shared_ptr<ShaderProgram> baseShader,
                               float topRadius, float bottomRadius,
                               float height, int rings, int segments)
        : StandardMesh(std::move(baseShader)) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        float halfHeight = height * 0.5f;

        auto addVertex = [&](glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) {
            Vertex v{};
            v.position = pos;
            v.normal = normal;
            v.color = {1.0f, 1.0f, 1.0f};
            v.texUV = uv;
            vertices.push_back(v);
        };

        // --- 1. Boční plášť ---
        for (int y = 0; y <= rings; ++y) {
            float t = static_cast<float>(y) / static_cast<float>(rings);
            float py = halfHeight - t * height;
            float radius = glm::mix(topRadius, bottomRadius, t);
            float vTex = 1.0f - t;

            for (int x = 0; x <= segments; ++x) {
                float phi = static_cast<float>(x) / static_cast<float>(segments) * glm::two_pi<float>();
                float px = radius * cos(phi);
                float pz = radius * sin(phi);

                glm::vec3 pos(px, py, pz);
                glm::vec3 normal = glm::normalize(glm::vec3(px, (bottomRadius - topRadius) / height, pz));
                glm::vec2 uv = {static_cast<float>(x) / static_cast<float>(segments), vTex};
                addVertex(pos, normal, uv);
            }
        }

        // --- 2. Horní a dolní základna ---
        auto addDisk = [&](const float y, const float radius, const bool isTop) {
            const int startIndex = static_cast<int>(vertices.size());
            const glm::vec3 normal = isTop ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
            const glm::vec3 center(0, y, 0);
            addVertex(center, normal, {0.5f, 0.5f});

            for (int x = 0; x <= segments; ++x) {
                const float phi = static_cast<float>(x) / static_cast<float>(segments) * glm::two_pi<float>();
                const float px = radius * cos(phi);
                const float pz = radius * sin(phi);
                const glm::vec3 pos(px, y, pz);
                const glm::vec2 uv = {0.5f + 0.5f * cos(phi), 0.5f + 0.5f * sin(phi)};
                addVertex(pos, normal, uv);
            }

            for (int x = 0; x < segments; ++x) {
                if (isTop) {
                    indices.push_back(startIndex);
                    indices.push_back(startIndex + x + 1);
                    indices.push_back(startIndex + x + 2);
                } else {
                    indices.push_back(startIndex);
                    indices.push_back(startIndex + x + 2);
                    indices.push_back(startIndex + x + 1);
                }
            }
        };

        // horní disk
        addDisk(+halfHeight, topRadius, true);
        // dolní disk
        addDisk(-halfHeight, bottomRadius, false);

        // --- 3. Indexy pro plášť ---
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

        // --- Výpočet tangentů / bitangentů ---
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
