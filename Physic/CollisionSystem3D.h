#ifndef SNAKE3_COLLISIONSYSTEM3D_H
#define SNAKE3_COLLISIONSYSTEM3D_H

#include "../Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include "Dynamics/DynamicBody.h"
#include <vector>

using namespace Model;
using namespace CollisionShape;
using namespace Dynamics;

namespace Physic {
    struct CollisionEntry {
        shared_ptr<CollisionShape3D> shapeNode;
        shared_ptr<MeshNode3D> parentObject;
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
        shared_ptr<MeshNode3D> node;
        shared_ptr<DynamicBody> body;
        // Cached at the start of each step() so resolveTopContact can tell whether
        // we just landed on a surface from above (vs penetrating it sideways).
        float previousBottomZ = 0.0f;
    };

    class CollisionSystem3D {
        std::vector<shared_ptr<MeshNode3D> > colliders;
        std::vector<CollisionEntry> flatEntries;
        std::vector<DynamicBodyEntry> dynamicBodies;
        glm::vec3 worldGravity{0.0f, 0.0f, -9.81f};
    public:
        void addCollider(const shared_ptr<MeshNode3D> &collider, bool isStatic = false);
        void update() const;
        void removeCollider(const std::shared_ptr<MeshNode3D> &collider);
        void clearColliders();

        void addDynamicBody(const shared_ptr<MeshNode3D> &node,
                            const shared_ptr<DynamicBody> &body);
        void removeDynamicBody(const shared_ptr<MeshNode3D> &node);
        void clearDynamicBodies();

        void setWorldGravity(const glm::vec3 &g) { worldGravity = g; }
        [[nodiscard]] const glm::vec3 &getWorldGravity() const { return worldGravity; }

        // One physics tick. Order: snapshot pre-state -> integrate enabled bodies ->
        // run broad/narrow phase (update()) -> resolve "landing from above" contacts.
        // Safe to call with no dynamic bodies registered (degenerates to plain update()).
        void step(float dt);
    private:
        static void resolveTopContact(const DynamicBodyEntry &entry) ;
    };
} // Physic

#endif //SNAKE3_COLLISIONSYSTEM3D_H