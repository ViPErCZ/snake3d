#ifndef SNAKE3_COLLISIONALGORITHMS_H
#define SNAKE3_COLLISIONALGORITHMS_H

#include <snake3d/Physic/CollisionSystem3D.h>

namespace Physic::Algorithms {
  bool BoxVsBox(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB);
  bool SphereVsSphere(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB);
  bool BoxVsSphere(const CollisionEntry& entBox, const CollisionEntry& entSphere, const glm::mat4& worldBox, const glm::mat4& worldSphere);
  bool SphereVsCapsule(const CollisionEntry& entSphere, const CollisionEntry& entCapsule, const glm::mat4& worldSphere, const glm::mat4& worldCapsule);
  bool CapsuleVsCapsule(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB);
  bool BoxVsCapsule(const CollisionEntry& entBox, const CollisionEntry& entCapsule, const glm::mat4& worldBox, const glm::mat4& worldCapsule);
  bool SphereVsCylinder(const CollisionEntry& entSphere, const CollisionEntry& entCylinder, const glm::mat4& worldSphere, const glm::mat4& worldCylinder);
  bool BoxVsCylinder(const CollisionEntry& entBox, const CollisionEntry& entCylinder, const glm::mat4& worldBox, const glm::mat4& worldCylinder);
  bool CapsuleVsCylinder(const CollisionEntry& entCapsule, const CollisionEntry& entCylinder, const glm::mat4& worldCapsule, const glm::mat4& worldCylinder);
  bool CylinderVsCylinder(const CollisionEntry& entA, const CollisionEntry& entB, const glm::mat4& worldA, const glm::mat4& worldB);
} // Algorithms
// Physic

#endif //SNAKE3_COLLISIONALGORITHMS_H