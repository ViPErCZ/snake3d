#ifndef SNAKE3_COLLISIONCHECK_H
#define SNAKE3_COLLISIONCHECK_H

#include <snake3d/Physic/CollisionSystem3D.h>

namespace Physic {
    class CollisionCheck {
    public:
        static bool IntersectAABB(const AABB& a, const AABB& b) {
            return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
                   (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
                   (a.min.z <= b.max.z && a.max.z >= b.min.z);
        }

        static bool IntersectExact(const CollisionEntry& a, const CollisionEntry& b);
    };
}

#endif //SNAKE3_COLLISIONCHECK_H