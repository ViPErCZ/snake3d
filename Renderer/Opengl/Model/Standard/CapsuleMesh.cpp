#include "../../../../Manager/VboIndexer.h"
#include "CapsuleMesh.h"

namespace Model {
    CapsuleMesh::CapsuleMesh(shared_ptr<BaseItem> baseItem, shared_ptr<ShaderManager> baseShader,
                             const float height, const float radius, const int rings,
                             const int segments) : StandardMesh(
        std::move(baseItem), std::move(baseShader)) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        float halfHeight = height * 0.5f;
        float cylinderHeight = height - 2.0f * radius;

        auto addVertex = [&](glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) {
            Vertex v{};
            v.position = pos;
            v.normal = normal;
            v.color = {1.0f, 1.0f, 1.0f};
            v.texUV = uv;
            vertices.push_back(v);
        };

        // --- 1. Horní polokoule (od vrcholu dolů k rovníku) ---
        for (int y = 0; y <= rings; ++y) {
            float theta = static_cast<float>(y) / static_cast<float>(rings) * glm::half_pi<float>(); // 0..pi/2
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            // Posun: horní polokoule nahoře
            float py = halfHeight - radius + radius * cosTheta;
            float vTex = 0.5f + 0.5f * sinTheta; // UV map

            for (int x = 0; x <= segments; ++x) {
                float phi = static_cast<float>(x) / static_cast<float>(segments) * glm::two_pi<float>();
                float px = radius * sinTheta * cos(phi);
                float pz = radius * sinTheta * sin(phi);

                glm::vec3 pos(px, py, pz);
                glm::vec3 normal = glm::normalize(pos - glm::vec3(0.0f, halfHeight - radius, 0.0f));
                addVertex(pos, normal, {static_cast<float>(x) / static_cast<float>(segments), vTex});
            }
        }

        // --- 2. Válec ---
        for (int y = 0; y <= 1; ++y) {
            float py = halfHeight - radius - static_cast<float>(y) * cylinderHeight; // horní = top polokoule rovník
            float vTex = 0.5f - 0.5f * static_cast<float>(y);

            for (int x = 0; x <= segments; ++x) {
                float phi = float(x) / static_cast<float>(segments) * glm::two_pi<float>();
                float px = radius * cos(phi);
                float pz = radius * sin(phi);
                glm::vec3 pos(px, py, pz);
                glm::vec3 normal = glm::normalize(glm::vec3(px, 0.0f, pz));
                addVertex(pos, normal, {float(x) / static_cast<float>(segments), vTex});
            }
        }

        // --- 3. Dolní polokoule (od dolíku nahoru k rovníku) ---
        for (int y = 0; y <= rings; ++y) {
            float theta = static_cast<float>(y) / static_cast<float>(rings) * glm::half_pi<float>(); // 0..pi/2
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            // Posun: dolní polokoule dole
            float py = -halfHeight + radius - radius * cosTheta;
            float vTex = 0.5f * sinTheta; // UV map

            for (int x = 0; x <= segments; ++x) {
                float phi = static_cast<float>(x) / static_cast<float>(segments) * glm::two_pi<float>();
                float px = radius * sinTheta * cos(phi);
                float pz = radius * sinTheta * sin(phi);

                glm::vec3 pos(px, py, pz);
                glm::vec3 normal = glm::normalize(pos - glm::vec3(0.0f, -halfHeight + radius, 0.0f));
                addVertex(pos, normal, {static_cast<float>(x) / static_cast<float>(segments), vTex});
            }
        }

        // --- Generování indexů ---
        auto buildIndices = [&](const int start, const int ringCount) {
            for (int y = 0; y < ringCount; ++y) {
                for (int x = 0; x < segments; ++x) {
                    GLuint i0 = start + y * (segments + 1) + x;
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
        };

        // Horní polokoule
        buildIndices(0, rings);
        // Válec
        buildIndices((rings + 1) * (segments + 1), 1);
        // Dolní polokoule
        int bottomStart = (rings + 1) * (segments + 1) + 2 * (segments + 1);
        buildIndices(bottomStart, rings);

        // --- Tangenty a bitangenty ---
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
