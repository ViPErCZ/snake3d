#ifndef SNAKE3_COLLISIONALGORITHMS_H
#define SNAKE3_COLLISIONALGORITHMS_H

#include "../CollisionSystem3D.h"

namespace Physic::Algorithms {
  bool BoxVsBox(const CollisionEntry& a, const CollisionEntry& b);
  bool SphereVsSphere(const CollisionEntry& entA, const CollisionEntry& entB);
  bool BoxVsSphere(const CollisionEntry& entBox, const CollisionEntry& entSphere);
  bool SphereVsCapsule(const CollisionEntry& entSphere, const CollisionEntry& entCapsule);
  bool CapsuleVsCapsule(const CollisionEntry& entA, const CollisionEntry& entB);
  bool BoxVsCapsule(const CollisionEntry& entBox, const CollisionEntry& entCapsule);
  bool SphereVsCylinder(const CollisionEntry& entSphere, const CollisionEntry& entCylinder);
  bool BoxVsCylinder(const CollisionEntry& entBox, const CollisionEntry& entCylinder);
  bool CapsuleVsCylinder(const CollisionEntry& entCapsule, const CollisionEntry& entCylinder);
  bool CylinderVsCylinder(const CollisionEntry& entA, const CollisionEntry& entB);
} // Algorithms
// Physic

#endif //SNAKE3_COLLISIONALGORITHMS_H