#include "SnakeSnapshotApplier.h"

#include <cmath>

namespace Net {
    namespace {
        void applyHeadVisualDirection(const std::shared_ptr<SnakeMeshNode3D> &snake,
                                      const SnakeMeshNode3D::eDIRECTION direction,
                                      const bool stopped) {
            if (!snake) {
                return;
            }

            SnakeMeshNode3D::eDIRECTION visualDirection = direction;
            if (visualDirection == SnakeMeshNode3D::NONE && snake->getChildren().empty()) {
                visualDirection = SnakeMeshNode3D::RIGHT;
            }
            if (visualDirection == SnakeMeshNode3D::NONE && !snake->getChildren().empty()) {
                if (const auto firstBody = std::dynamic_pointer_cast<SnakeMeshNode3D>(snake->getChildren().front())) {
                    const auto headPos = snake->getPosition();
                    const auto bodyPos = firstBody->getPosition();
                    visualDirection = SnakeMeshNode3D::RIGHT; // default (body is to the left of the head)
                    if (bodyPos.x > headPos.x) {
                        visualDirection = SnakeMeshNode3D::LEFT;
                    } else if (bodyPos.y > headPos.y) {
                        visualDirection = SnakeMeshNode3D::DOWN;
                    } else if (bodyPos.y < headPos.y) {
                        visualDirection = SnakeMeshNode3D::UP;
                    }
                }
            }

            snake->setDirection(stopped ? SnakeMeshNode3D::NONE : direction);
            snake->setRotationX(90.0f);
            switch (visualDirection) {
                case SnakeMeshNode3D::LEFT:
                    snake->setRotationY(180.0f);
                    break;
                case SnakeMeshNode3D::UP:
                    snake->setRotationY(90.0f);
                    break;
                case SnakeMeshNode3D::DOWN:
                    snake->setRotationY(-90.0f);
                    break;
                case SnakeMeshNode3D::RIGHT:
                case SnakeMeshNode3D::NONE:
                default:
                    snake->setRotationY(0.0f);
                    break;
            }
        }

        void ensureSnakeLength(const std::shared_ptr<SnakeMeshNode3D> &snake, const size_t segmentCount) {
            if (!snake || segmentCount == 0) {
                return;
            }

            const size_t currentSegmentCount = snake->getChildren().size() + 1;
            if (currentSegmentCount > segmentCount) {
                snake->respawn();
            }

            while (snake->getChildren().size() + 1 < segmentCount) {
                snake->addTile(SnakeMeshNode3D::RIGHT);
            }
        }
    } // namespace

    int worldToVirtualCoord(const float worldCoord) {
        return static_cast<int>(std::lround((worldCoord + 23.0f) * 16.0f)) + 16;
    }

    std::vector<glm::vec3> buildStraightSnakePositions(const glm::vec3 &headPosition,
                                                       const size_t segmentCount,
                                                       const SnakeMeshNode3D::eDIRECTION direction) {
        std::vector<glm::vec3> positions;
        positions.reserve(segmentCount);
        positions.push_back(headPosition);

        glm::vec3 tailDelta{-2.0f, 0.0f, 0.0f};
        switch (direction) {
            case SnakeMeshNode3D::LEFT:
                tailDelta = {2.0f, 0.0f, 0.0f};
                break;
            case SnakeMeshNode3D::RIGHT:
            case SnakeMeshNode3D::NONE:
                tailDelta = {-2.0f, 0.0f, 0.0f};
                break;
            case SnakeMeshNode3D::UP:
                tailDelta = {0.0f, -2.0f, 0.0f};
                break;
            case SnakeMeshNode3D::DOWN:
                tailDelta = {0.0f, 2.0f, 0.0f};
                break;
            default:
                break;
        }

        for (size_t index = 1; index < segmentCount; ++index) {
            positions.emplace_back(headPosition + tailDelta * static_cast<float>(index));
        }
        return positions;
    }

    void applyExactSnakePositions(const std::shared_ptr<SnakeMeshNode3D> &snake,
                                  const std::vector<glm::vec3> &positions,
                                  const SnakeMeshNode3D::eDIRECTION direction,
                                  const bool stopped) {
        if (!snake || positions.empty()) {
            return;
        }

        ensureSnakeLength(snake, positions.size());

        const auto &headPosition = positions.front();
        snake->setPosition({headPosition.x, headPosition.y, headPosition.z});
        snake->x = worldToVirtualCoord(headPosition.x);
        snake->y = worldToVirtualCoord(headPosition.y);

        size_t index = 1;
        for (const auto &child : snake->getChildren()) {
            const auto tile = std::dynamic_pointer_cast<SnakeMeshNode3D>(child);
            if (!tile || index >= positions.size()) {
                continue;
            }

            const auto &position = positions[index];
            tile->setPosition({position.x, position.y, position.z});
            tile->x = worldToVirtualCoord(position.x);
            tile->y = worldToVirtualCoord(position.y);
            tile->setDirection(SnakeMeshNode3D::NONE);
            ++index;
        }

        applyHeadVisualDirection(snake, direction, stopped);
    }
} // Net
