#ifndef SNAKE3_NETGAMESNAPSHOT_H
#define SNAKE3_NETGAMESNAPSHOT_H

#include <vector>

#include <glm/vec3.hpp>

#include "../../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"
#include <snake3d/Network/NetProtocol.h>

namespace Net {
    struct SnakeSnapshotState {
        // Per-segment world position. Z is replicated so clients see jump arcs
        // and falls through holes the same way the server does.
        std::vector<glm::vec3> positions;
        Model::SnakeMeshNode3D::eDIRECTION direction = Model::SnakeMeshNode3D::NONE;
        uint32_t segmentCount = 0;
        uint32_t respawnSerial = 0;
        bool crashActive = false;
        bool stopped = false;
    };

    struct WorldSnapshotState {
        SnakeSnapshotState localSnake;
        SnakeSnapshotState remoteSnake;
        float coinX = 0.0f;
        float coinY = 0.0f;
        bool coinVisible = false;
        uint32_t level = 0;
        uint32_t eatCounter = 0;
        uint32_t lives = 0;
        bool winning = false;
    };

    uint8_t encodeNetDirection(Model::SnakeMeshNode3D::eDIRECTION direction);
    Model::SnakeMeshNode3D::eDIRECTION decodeNetDirection(uint8_t value);

    void writeSnakeSnapshotState(BufferWriter &writer, const SnakeSnapshotState &state);
    bool readSnakeSnapshotState(BufferReader &reader, SnakeSnapshotState &state);

    void writeWorldSnapshotState(BufferWriter &writer, const WorldSnapshotState &state);
    bool readWorldSnapshotState(BufferReader &reader, WorldSnapshotState &state);
} // Net

#endif // SNAKE3_NETGAMESNAPSHOT_H
