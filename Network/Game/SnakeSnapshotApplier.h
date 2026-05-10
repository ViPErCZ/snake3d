#ifndef SNAKE3_SNAKESNAPSHOTAPPLIER_H
#define SNAKE3_SNAKESNAPSHOTAPPLIER_H

#include <vector>

#include "../../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"

namespace Net {
    int worldToVirtualCoord(float worldCoord);

    std::vector<glm::vec2> buildStraightSnakePositions(const glm::vec2 &headPosition,
                                                       size_t segmentCount,
                                                       SnakeMeshNode3D::eDIRECTION direction);

    void applyExactSnakePositions(const std::shared_ptr<SnakeMeshNode3D> &snake,
                                  const std::vector<glm::vec2> &positions,
                                  SnakeMeshNode3D::eDIRECTION direction,
                                  bool stopped);
} // Net

#endif // SNAKE3_SNAKESNAPSHOTAPPLIER_H
