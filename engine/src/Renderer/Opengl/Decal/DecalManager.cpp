#include <snake3d/Renderer/Opengl/Decal/DecalManager.h>

#include <algorithm>
#include <cmath>
#include <string>

#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Manager/TextureManager.h>
#include <snake3d/Renderer/Opengl/Material/ShaderMaterial.h>
#include <snake3d/Renderer/Opengl/Material/TextureSlots.h>
#include <snake3d/Renderer/Opengl/Material/Uniform/CallbackUniform.h>
#include <snake3d/Renderer/Opengl/Model/Standard/ArrayMesh.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Mesh.h>
#include <snake3d/Tools/Blending.h>

using namespace std;
using namespace Model;

namespace Renderer {
    namespace {
        void clipHalf(vector<glm::vec3>& poly, const glm::vec3& axis, const glm::vec3& c, const float half) {
            if (poly.empty()) return;
            vector<glm::vec3> out;
            out.reserve(poly.size() + 4);
            const size_t n = poly.size();
            for (size_t i = 0; i < n; ++i) {
                const glm::vec3& cur = poly[i];
                const glm::vec3& prev = poly[(i + n - 1) % n];
                const float dCur = glm::dot(cur - c, axis) - half;
                const float dPrev = glm::dot(prev - c, axis) - half;
                if (dCur <= 0.0f) {                       // cur inside
                    if (dPrev > 0.0f) {                   // entering -> add crossing
                        const float t = dPrev / (dPrev - dCur);
                        out.push_back(prev + (cur - prev) * t);
                    }
                    out.push_back(cur);
                } else if (dPrev <= 0.0f) {               // leaving -> add crossing
                    const float t = dPrev / (dPrev - dCur);
                    out.push_back(prev + (cur - prev) * t);
                }
            }
            poly.swap(out);
        }
    }

    void DecalManager::init(const shared_ptr<Tools::ContextState>& contextState,
                            const shared_ptr<Manager::ResourceManager>& resourceManager,
                            const shared_ptr<Manager::ShaderProgram>& shader,
                            const shared_ptr<Manager::TextureManager>& texture,
                            const int poolSize, const float depthBias) {
        nodes_.reserve(poolSize);
        meshes_.reserve(poolSize);
        const vector<ModelUtils::Vertex> seed(3);
        const vector<unsigned int> seedIdx{0, 1, 2};
        for (int i = 0; i < poolSize; ++i) {
            auto material = make_shared<Material::ShaderMaterial>(shader);
            material->setBlending(Tools::Blending::Translucent);
            material->setDepthBias(depthBias);
            material->setUniform("tex", make_shared<Uniform::CallbackUniform>(
                [texture](const string& name, const shared_ptr<Manager::ShaderProgram>& sh) {
                    if (texture) texture->bind(Material::TextureSlots::Albedo);
                    sh->setInt(name, Material::TextureSlots::Albedo);
                    sh->setVec3("uTint", glm::vec3(1.0f)); // explicit: GLSL defaults aren't honored everywhere
                    sh->setFloat("uAlpha", 1.0f);
                }));
            auto mesh = make_shared<ArrayMesh>(shader);
            mesh->fromMesh(make_shared<ModelUtils::Mesh>(seed, seedIdx));
            mesh->setMaterial(material);
            mesh->setCullBackFace(false);
            auto node = make_shared<MeshNode3D>(contextState, mesh, resourceManager);
            node->setVisible(false);
            nodes_.push_back(node);
            meshes_.push_back(mesh);
        }
    }

    void DecalManager::spawn(const glm::vec3& point, const glm::vec3& normalRaw, const float size, const float rotationDeg) {
        if (nodes_.empty() || receivers_.empty()) return;
        const float nlen = glm::length(normalRaw);
        const glm::vec3 n = nlen > 1e-5f ? normalRaw / nlen : glm::vec3(0, 0, 1);

        // Tangent frame on the surface, rotated by rotationDeg for per-hit variety.
        glm::vec3 up = std::fabs(n.z) < 0.9f ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
        glm::vec3 t = glm::normalize(glm::cross(up, n));
        glm::vec3 b = glm::cross(n, t);
        const float a = glm::radians(rotationDeg);
        const glm::vec3 tr = t * std::cos(a) + b * std::sin(a);
        const glm::vec3 br = -t * std::sin(a) + b * std::cos(a);
        t = tr; b = br;

        const float half = size * 0.5f;
        const float depth = size * 0.6f;          // how far along the normal the projection reaches
        const float reach = size * 0.9f;          // broad-phase radius (corner + half)

        vector<ModelUtils::Vertex> vertices;
        vector<unsigned int> idx;
        for (const auto& tri : receivers_) {
            // Broad phase via bounding sphere (centroid + radius) so a LARGE triangle whose
            // vertices are all far but whose face covers the hit point isn't wrongly skipped.
            const glm::vec3 cen = (tri[0] + tri[1] + tri[2]) * (1.0f / 3.0f);
            const float triR = std::max({glm::length(tri[0] - cen), glm::length(tri[1] - cen),
                                         glm::length(tri[2] - cen)});
            if (glm::length(point - cen) > reach + triR) continue;
            // Only project onto faces roughly PARALLEL to the decal plane (perpendicular to n),
            // regardless of winding -> abs(dot). Skips faces seen edge-on so a decal on one wall
            // doesn't smear onto a perpendicular one through the box.
            const glm::vec3 fn = glm::cross(tri[1] - tri[0], tri[2] - tri[0]);
            if (glm::length(fn) < 1e-9f) continue;
            if (std::fabs(glm::dot(glm::normalize(fn), n)) < 0.25f) continue;

            vector<glm::vec3> poly{tri[0], tri[1], tri[2]};
            clipHalf(poly, t, point, half);  clipHalf(poly, -t, point, half);
            clipHalf(poly, b, point, half);  clipHalf(poly, -b, point, half);
            clipHalf(poly, n, point, depth); clipHalf(poly, -n, point, depth);
            if (poly.size() < 3) continue;

            const auto base = static_cast<unsigned int>(vertices.size());
            for (const auto& v : poly) {
                ModelUtils::Vertex vert{};
                vert.position = v;
                vert.normal = n;
                vert.color = glm::vec3(1.0f);
                vert.texUV = glm::vec2(glm::dot(v - point, t) / (2.0f * half) + 0.5f,
                                       glm::dot(v - point, b) / (2.0f * half) + 0.5f);
                vertices.push_back(vert);
            }
            for (size_t k = 1; k + 1 < poly.size(); ++k) { // fan triangulation
                idx.push_back(base);
                idx.push_back(base + static_cast<unsigned int>(k));
                idx.push_back(base + static_cast<unsigned int>(k + 1));
            }
        }
        if (idx.empty()) return; // no receiver surface inside the box -> no decal (don't levitate)

        auto& mesh = meshes_[next_];
        auto& node = nodes_[next_];
        next_ = (next_ + 1) % nodes_.size();
        mesh->fromMesh(make_shared<ModelUtils::Mesh>(vertices, idx));
        node->setVisible(true);
    }
} // namespace Renderer
