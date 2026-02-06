#ifndef SNAKE3_COLLISIONSYSTEM3D_H
#define SNAKE3_COLLISIONSYSTEM3D_H

#include "../Renderer/Opengl/Model/Collision/CollisionShape3D.h"
#include "../Renderer/Opengl/Model/Standard/MeshNode3D.h"
#include <vector>

using namespace Model;
using namespace CollisionShape;

namespace Physic {
    struct CollisionEntry {
        shared_ptr<CollisionShape3D> shapeNode;
        shared_ptr<MeshNode3D> parentObject;
    };

    class CollisionSystem3D {
        std::vector<shared_ptr<MeshNode3D> > colliders;
        std::vector<CollisionEntry> flatEntries;
    public:
        void addCollider(const shared_ptr<MeshNode3D> &collider);
        void update() const;
        void removeCollider(const std::shared_ptr<MeshNode3D> &collider);
        void clearColliders();
    };
} // Physic

#endif //SNAKE3_COLLISIONSYSTEM3D_H