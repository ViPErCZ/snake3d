#include "CollisionCheck.h"

#include "Algorithms/CollisionAlgorithms.h"

namespace Physic {
    bool CollisionCheck::IntersectExact(const CollisionEntry &a, const CollisionEntry &b) {
        const glm::mat4 worldMatrixA = a.parentObject->getWorldMatrix() * a.shapeNode->getModelMatrix();
        const glm::mat4 worldMatrixB = b.parentObject->getWorldMatrix() * b.shapeNode->getModelMatrix();

        const ShapeType typeA = a.shapeNode->getShape()->getType();
        const ShapeType typeB = b.shapeNode->getShape()->getType();

        if (typeA > typeB) {
            return IntersectExact(b, a);
        }

        switch (typeA) {
            case ShapeType::Box:
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsBox(a, b, worldMatrixA, worldMatrixB);
                if (typeB == ShapeType::Sphere)
                    return Algorithms::BoxVsSphere(a, b, worldMatrixA, worldMatrixB);
                if (typeB == ShapeType::Capsule)
                    return Algorithms::BoxVsCapsule(a, b, worldMatrixA, worldMatrixB);
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::BoxVsCylinder(a, b, worldMatrixA, worldMatrixB);
                break;

            case ShapeType::Sphere:
                 if (typeB == ShapeType::Sphere)
                     return Algorithms::SphereVsSphere(a, b, worldMatrixA, worldMatrixB);
                 if (typeB == ShapeType::Capsule)
                     return Algorithms::SphereVsCapsule(a, b, worldMatrixA, worldMatrixB);
                 if (typeB == ShapeType::Cylinder)
                     return Algorithms::SphereVsCylinder(a, b, worldMatrixA, worldMatrixB);
                 break;

            case ShapeType::Capsule:
                if (typeB == ShapeType::Capsule)
                    return Algorithms::CapsuleVsCapsule(a, b, worldMatrixA, worldMatrixB);
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsCapsule(b, a, worldMatrixB, worldMatrixA);
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::CapsuleVsCylinder(a, b, worldMatrixA, worldMatrixB);
                break;

            case ShapeType::Cylinder:
                if (typeB == ShapeType::Cylinder)
                    return Algorithms::CylinderVsCylinder(a, b, worldMatrixA, worldMatrixB);
                if (typeB == ShapeType::Box)
                    return Algorithms::BoxVsCylinder(b, a, worldMatrixB, worldMatrixA);
                if (typeB == ShapeType::Sphere)
                    return Algorithms::SphereVsCylinder(b, a, worldMatrixB, worldMatrixA);
                if (typeB == ShapeType::Capsule)
                    return Algorithms::CapsuleVsCylinder(b, a, worldMatrixB, worldMatrixA);
                break;
        }

        return false;
    }
}
