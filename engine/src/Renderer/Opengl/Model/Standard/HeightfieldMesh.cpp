#include <snake3d/Renderer/Opengl/Model/Standard/HeightfieldMesh.h>

#include <algorithm>
#include <utility>

#include <snake3d/Manager/VboIndexer.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Mesh.h>

using namespace Manager;
using namespace ModelUtils;
using namespace std;

namespace Model {
    HeightfieldMesh::HeightfieldMesh(shared_ptr<ShaderProgram> baseShader,
                                     const float halfSpan, const int gridCells, HeightFn height)
        : StandardMesh(std::move(baseShader)), height(std::move(height)) {
        const int n = std::max(2, gridCells);
        const float span = 2.0f * halfSpan;
        const float cell = span / static_cast<float>(n);
        cells = n;
        half = halfSpan;
        gridHeights.reserve(static_cast<size_t>(n + 1) * (n + 1));

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        vertices.reserve(static_cast<size_t>(n + 1) * (n + 1));

        // (n+1)x(n+1) grid of vertices over [-halfSpan, halfSpan]^2 in XZ, y = height.
        for (int iz = 0; iz <= n; ++iz) {
            const float z = -halfSpan + static_cast<float>(iz) * cell;
            for (int ix = 0; ix <= n; ++ix) {
                const float x = -halfSpan + static_cast<float>(ix) * cell;
                Vertex v{};
                v.position = {x, this->height(x, z), z};
                gridHeights.push_back(v.position.y);
                // Smooth normal from central differences of the height field.
                const float hl = this->height(x - cell, z), hr = this->height(x + cell, z);
                const float hd = this->height(x, z - cell), hu = this->height(x, z + cell);
                v.normal = glm::normalize(glm::vec3(hl - hr, 2.0f * cell, hd - hu));
                v.color = {1.0f, 1.0f, 1.0f};
                v.texUV = {static_cast<float>(ix), static_cast<float>(iz)}; // world-tile UV
                vertices.push_back(v);
                localMin = glm::min(localMin, v.position);
                localMax = glm::max(localMax, v.position);
            }
        }

        // Two triangles per quad (CCW, +Y up - matches PlaneMesh winding so the existing
        // front-face/shadow setup is unchanged). heightAtMesh interpolates with this
        // exact diagonal - keep the two in sync.
        const int stride = n + 1;
        indices.reserve(static_cast<size_t>(n) * n * 6);
        for (int iz = 0; iz < n; ++iz) {
            for (int ix = 0; ix < n; ++ix) {
                const GLuint i0 = iz * stride + ix;
                const GLuint i1 = i0 + 1;
                const GLuint i2 = i0 + stride;
                const GLuint i3 = i2 + 1;
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
        }

        std::vector<glm::vec3> positions, normals;
        std::vector<glm::vec2> uvs;
        positions.reserve(vertices.size());
        normals.reserve(vertices.size());
        uvs.reserve(vertices.size());
        for (const auto &v : vertices) {
            positions.push_back(v.position);
            normals.push_back(v.normal);
            uvs.push_back(v.texUV);
        }
        std::vector<glm::vec3> tangents, biTangents;
        VboIndexer::computeTangentBasis(positions, uvs, normals, tangents, biTangents);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].tangents = tangents[i];
            vertices[i].biTangents = biTangents[i];
        }

        mesh = std::make_shared<Mesh>(vertices, indices, false, "HeightfieldMesh");
    }

    float HeightfieldMesh::heightAt(const float x, const float z) const {
        return height ? height(x, z) : 0.0f;
    }

    float HeightfieldMesh::heightAtMesh(const float x, const float z) const {
        if (gridHeights.empty()) return heightAt(x, z);
        const int n = cells;
        const float cell = 2.0f * half / static_cast<float>(n);
        const float gx = glm::clamp((x + half) / cell, 0.0f, static_cast<float>(n) - 1e-4f);
        const float gz = glm::clamp((z + half) / cell, 0.0f, static_cast<float>(n) - 1e-4f);
        const int ix = static_cast<int>(gx), iz = static_cast<int>(gz);
        const float fx = gx - static_cast<float>(ix), fz = gz - static_cast<float>(iz);
        const int s = n + 1;
        const float h00 = gridHeights[iz * s + ix], h10 = gridHeights[iz * s + ix + 1];
        const float h01 = gridHeights[(iz + 1) * s + ix], h11 = gridHeights[(iz + 1) * s + ix + 1];
        // Same diagonal split as the index buffer: (i0,i2,i1) covers fx+fz<=1,
        // (i1,i2,i3) the rest - plane interpolation per triangle, not bilinear.
        return fx + fz <= 1.0f
                   ? h00 + fx * (h10 - h00) + fz * (h01 - h00)
                   : h11 + (1.0f - fx) * (h01 - h11) + (1.0f - fz) * (h10 - h11);
    }

    glm::vec3 HeightfieldMesh::normalAt(const float x, const float z, const float eps) const {
        const float hl = heightAt(x - eps, z), hr = heightAt(x + eps, z);
        const float hd = heightAt(x, z - eps), hu = heightAt(x, z + eps);
        return glm::normalize(glm::vec3(hl - hr, 2.0f * eps, hd - hu));
    }
} // Model
