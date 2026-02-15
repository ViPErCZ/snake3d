#include "CollisionCheck.h"

#include "Algorithms/CollisionAlgorithms.h"

namespace Physic {
    bool CollisionCheck::IntersectExact(const CollisionEntry &a, const CollisionEntry &b) {
        const ShapeType typeA = a.shapeNode->getShape()->getType();
        const ShapeType typeB = b.shapeNode->getShape()->getType();

        if (typeA > typeB) {
            return IntersectExact(b, a);
        }

        switch (typeA) {
            case ShapeType::Box:
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsBox(a, b);
                if (typeB == ShapeType::Sphere)
                    return Algorithms::BoxVsSphere(a, b);
                break;

            case ShapeType::Sphere:
                 if (typeB == ShapeType::Sphere)
                     return Algorithms::SphereVsSphere(a, b);
                 break;
        }

        return false;
    }
}
