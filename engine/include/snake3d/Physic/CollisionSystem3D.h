#ifndef SNAKE3_COLLISIONSYSTEM3D_H
#define SNAKE3_COLLISIONSYSTEM3D_H

#include <snake3d/Renderer/Opengl/Model/Collision/CollisionShape3D.h>
#include <snake3d/Renderer/Opengl/Model/Standard/MeshNode3D.h>
#include <snake3d/Physic/Dynamics/DynamicBody.h>
#include <optional>
#include <vector>

namespace Physic {
    struct CollisionEntry {
        std::shared_ptr<CollisionShape::CollisionShape3D> shapeNode;
        std::shared_ptr<Model::MeshNode3D> parentObject;
        // Static entries (floor cells, level walls) never move - they skip
        // pair-tests against each other and reuse their world AABB across frames.
        bool isStatic = false;
        mutable bool aabbCached = false;
        mutable AABB cachedAABB{};
        // Per-frame cache populated in update() AABB phase - inline copy z
        // shapeNode getters aby pair loop nedělal 3 pointer-chases per check
        // (parent→shape→isCollisionEnabled / getCollisionLayer / getMask).
        // 549k×3×2 = 3M pointer chases je hlavní cost při velkém scene grafu.
        mutable bool cachedEnabled = false;
        mutable uint32_t cachedLayer = 0;
        mutable uint32_t cachedMask = 0;
    };

    struct DynamicBodyEntry {
        std::shared_ptr<Model::MeshNode3D> node;
        std::shared_ptr<Dynamics::DynamicBody> body;
        // Cached at the start of each step() so resolveTopContact can tell whether
        // we just landed on a surface from above (vs penetrating it sideways).
        float previousBottomZ = 0.0f;
    };

    // One overlap result from a scene query (overlapSphere / overlapAABB). The
    // engine reports WHAT was overlapped and by how much; the CALLER decides the
    // response (push out, bounce, despawn, ...). `normal`/`depth` are the minimum
    // translation that would separate the probe from the hit collider; `contact`
    // is an approximate point on the collider surface (e.g. for impact effects).
    struct OverlapHit {
        std::shared_ptr<Model::MeshNode3D> node;                       // hit collider's parent object
        std::shared_ptr<CollisionShape::CollisionShape3D> shape;       // the hit shape node
        AABB worldAABB{};        // hit collider's world AABB (handy for callers' own interval math)
        glm::vec3 normal{0.0f};  // unit push-out direction (from the collider toward the probe)
        float depth{0.0f};       // penetration depth along `normal`
        glm::vec3 contact{0.0f}; // approximate contact point on the collider surface
    };

    // One ray-cast result (nearest hit). Mirrors OverlapHit's "what was hit"
    // reporting for the read-only ray query below.
    struct RaycastHit {
        std::shared_ptr<Model::MeshNode3D> node;                 // hit collider's parent object
        std::shared_ptr<CollisionShape::CollisionShape3D> shape; // the hit shape node
        glm::vec3 point{0.0f};   // world-space first intersection
        glm::vec3 normal{0.0f};  // axis-aligned normal of the entered AABB face
        float dist{0.0f};        // distance from origin along `dir`
    };

    class CollisionSystem3D {
        std::vector<std::shared_ptr<Model::MeshNode3D> > colliders;
        std::vector<CollisionEntry> flatEntries;
        std::vector<DynamicBodyEntry> dynamicBodies;
        glm::vec3 worldGravity{0.0f, 0.0f, -9.81f};
    public:
        void addCollider(const std::shared_ptr<Model::MeshNode3D> &collider, bool isStatic = false);
        void update() const;
        void removeCollider(const std::shared_ptr<Model::MeshNode3D> &collider);
        void clearColliders();

        void addDynamicBody(const std::shared_ptr<Model::MeshNode3D> &node,
                            const std::shared_ptr<Dynamics::DynamicBody> &body);
        void removeDynamicBody(const std::shared_ptr<Model::MeshNode3D> &node);
        void clearDynamicBodies();

        void setWorldGravity(const glm::vec3 &g) { worldGravity = g; }
        [[nodiscard]] const glm::vec3 &getWorldGravity() const { return worldGravity; }

        // One physics tick. Order: snapshot pre-state -> integrate enabled bodies ->
        // run broad/narrow phase (update()) -> resolve "landing from above" contacts.
        // Safe to call with no dynamic bodies registered (degenerates to plain update()).
        void step(float dt);

        // Vrátí DynamicBody zaregistrovaný pro daný node, nebo nullptr když node
        // dynamic body nemá. Lineární scan přes dynamicBodies (≤ desítky v praxi),
        // určeno pro ad-hoc lookups z debug UI - ne pro hot path.
        [[nodiscard]] std::shared_ptr<Dynamics::DynamicBody>
            findDynamicBody(const std::shared_ptr<Model::MeshNode3D>& node) const;

        // --- Scene queries (additive; independent of step()/update()) --------
        // Test a PROBE shape against every registered collider whose collision
        // LAYER intersects `layerMask` (default = all layers). These are read-only
        // broad-phase (world-AABB) tests: exact for axis-aligned box colliders
        // (an OBB is approximated by its world AABB). They never mutate body/shape
        // state, so they are safe to call any time, independent of step()/update().
        // The probe itself has no layer and is never returned. Returns one
        // OverlapHit per overlapping collider (penetration + contact filled in).

        // Sphere probe (center + radius). `normal`/`contact` come from the closest
        // point on each collider's world AABB; `depth` = radius - distance.
        [[nodiscard]] std::vector<OverlapHit>
            overlapSphere(const glm::vec3 &center, float radius, std::uint32_t layerMask = ~0u) const;

        // Axis-aligned box probe. `normal`/`depth` are the minimum-translation axis
        // of the AABB overlap; `contact` is the probe center clamped into the collider.
        [[nodiscard]] std::vector<OverlapHit>
            overlapAABB(const AABB &box, std::uint32_t layerMask = ~0u) const;

        // Cast a ray from `origin` along `dir` (need not be normalized) up to
        // `maxDist`, against every collider whose layer intersects `layerMask`.
        // Returns the NEAREST hit, or std::nullopt if nothing is hit. Read-only
        // ray-vs-(world)AABB slab test, consistent with the overlap queries'
        // OBB-as-world-AABB approximation; never mutates body/shape state, so it
        // is safe to call any time (independent of step()/update()).
        [[nodiscard]] std::optional<RaycastHit>
            raycast(const glm::vec3 &origin, const glm::vec3 &dir,
                    float maxDist, std::uint32_t layerMask = ~0u) const;
    private:
        static void resolveTopContact(const DynamicBodyEntry &entry) ;
    };
} // Physic

#endif //SNAKE3_COLLISIONSYSTEM3D_H