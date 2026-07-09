#ifndef SNAKE3_DECALMANAGER_H
#define SNAKE3_DECALMANAGER_H

#include <array>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

namespace Manager { class ResourceManager; class ShaderProgram; class TextureManager; }
namespace Tools { class ContextState; }
namespace Model { class MeshNode3D; class ArrayMesh; }

namespace Renderer {

    // E3: projected decal system. Instead of a flat billboard quad (which floats off / hangs
    // past silhouette edges), a decal is PROJECTED onto the receiver geometry: an oriented box
    // around the hit point is clipped against the nearby world triangles (Sutherland-Hodgman),
    // producing decal geometry that conforms to the surface and STOPS exactly where the surface
    // does -- so it never overhangs an edge. Rendered with a sprite-style shader (samples `tex`,
    // `uTint`, `uAlpha`) + a depth bias so it sits on the surface without z-fighting.
    //
    // Usage: init() once, setReceivers() with the static world triangles to project onto, add
    // nodes() to a Scene for rendering, then spawn() per impact. A ring buffer recycles the
    // oldest decal, so decals persist cheaply.
    class DecalManager {
    public:
        void init(const std::shared_ptr<Tools::ContextState>& contextState,
                  const std::shared_ptr<Manager::ResourceManager>& resourceManager,
                  const std::shared_ptr<Manager::ShaderProgram>& shader,
                  const std::shared_ptr<Manager::TextureManager>& texture,
                  int poolSize, float depthBias);

        // World-space triangles the decals project onto (buildings, walls, rocks, ...).
        void setReceivers(std::vector<std::array<glm::vec3, 3>> tris) { receivers_ = std::move(tris); }

        // Pool nodes — the owner adds these to a Scene so they get rendered.
        [[nodiscard]] const std::vector<std::shared_ptr<Model::MeshNode3D>>& nodes() const { return nodes_; }

        // Project a decal at a world hit point. `size` = decal width/height in world units;
        // `rotationDeg` rotates the texture around the surface normal (pass a per-hit random
        // value for variety). No-op if no receiver geometry falls inside the decal box.
        void spawn(const glm::vec3& point, const glm::vec3& normal, float size, float rotationDeg);

    private:
        std::vector<std::array<glm::vec3, 3>> receivers_;
        std::vector<std::shared_ptr<Model::MeshNode3D>> nodes_;
        std::vector<std::shared_ptr<Model::ArrayMesh>> meshes_;
        std::size_t next_ = 0;
    };

} // namespace Renderer

#endif // SNAKE3_DECALMANAGER_H
