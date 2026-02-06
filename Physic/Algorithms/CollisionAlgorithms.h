#ifndef SNAKE3_COLLISIONALGORITHMS_H
#define SNAKE3_COLLISIONALGORITHMS_H

#include "../CollisionSystem3D.h"

namespace Physic::Algorithms {
  bool BoxVsBox(const CollisionEntry& a, const CollisionEntry& b);
  bool SphereVsSphere(const CollisionEntry& a, const CollisionEntry& b);
  bool BoxVsSphere(const CollisionEntry& a, const CollisionEntry& b);
} // Algorithms
// Physic

#endif //SNAKE3_COLLISIONALGORITHMS_H