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
                if (typeB == ShapeType::Capsule)
                    return Algorithms::BoxVsCapsule(a, b);
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::BoxVsCylinder(a, b);
                break;

            case ShapeType::Sphere:
                 if (typeB == ShapeType::Sphere)
                     return Algorithms::SphereVsSphere(a, b);
                 if (typeB == ShapeType::Capsule)
                     return Algorithms::SphereVsCapsule(a, b);
                 if (typeB == ShapeType::Cylinder)
                     return Algorithms::SphereVsCylinder(a, b);
                 break;

            case ShapeType::Capsule:
                if (typeB == ShapeType::Capsule)
                    return Algorithms::CapsuleVsCapsule(a, b);
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsCapsule(b, a);
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::CapsuleVsCylinder(a, b);
                break;

            case ShapeType::Cylinder:
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::CylinderVsCylinder(a, b);
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsCylinder(b, a);
                if (typeB == ShapeType::Sphere)
                    return Algorithms::SphereVsCylinder(b, a);
                if (typeB == ShapeType::Capsule)
                    return Algorithms::CapsuleVsCylinder(b, a);
                break;
        }

        return false;
    }
}
