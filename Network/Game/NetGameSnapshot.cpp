#include "NetGameSnapshot.h"

namespace Net {
    uint8_t encodeNetDirection(const SnakeMeshNode3D::eDIRECTION direction) {
        return direction == SnakeMeshNode3D::NONE ? 255u : static_cast<uint8_t>(direction);
    }

    SnakeMeshNode3D::eDIRECTION decodeNetDirection(const uint8_t value) {
        return value == 255
                   ? SnakeMeshNode3D::NONE
                   : static_cast<SnakeMeshNode3D::eDIRECTION>(value);
    }

    void writeSnakeSnapshotState(BufferWriter &writer, const SnakeSnapshotState &state) {
        writer.writeU32(static_cast<uint32_t>(state.positions.size()));
        for (const auto &position : state.positions) {
            writer.writeF32(position.x);
            writer.writeF32(position.y);
            writer.writeF32(position.z);
        }
        writer.writeU8(encodeNetDirection(state.direction));
        writer.writeU32(state.segmentCount);
        writer.writeU32(state.respawnSerial);
        writer.writeU8(state.crashActive ? 1u : 0u);
        writer.writeU8(state.stopped ? 1u : 0u);
    }

    bool readSnakeSnapshotState(BufferReader &reader, SnakeSnapshotState &state) {
        uint32_t positionCount = 0;
        uint8_t direction = 0;
        uint8_t crashActive = 0;
        uint8_t stopped = 0;
        if (!reader.readU32(positionCount)) {
            return false;
        }

        state.positions.clear();
        state.positions.reserve(positionCount);
        for (uint32_t i = 0; i < positionCount; ++i) {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            if (!reader.readF32(x) || !reader.readF32(y) || !reader.readF32(z)) {
                return false;
            }
            state.positions.emplace_back(x, y, z);
        }

        if (!reader.readU8(direction) ||
            !reader.readU32(state.segmentCount) ||
            !reader.readU32(state.respawnSerial) ||
            !reader.readU8(crashActive) ||
            !reader.readU8(stopped)) {
            return false;
        }

        state.direction = decodeNetDirection(direction);
        state.crashActive = crashActive != 0;
        state.stopped = stopped != 0;
        return true;
    }

    void writeWorldSnapshotState(BufferWriter &writer, const WorldSnapshotState &state) {
        writeSnakeSnapshotState(writer, state.localSnake);
        writeSnakeSnapshotState(writer, state.remoteSnake);
        writer.writeF32(state.coinX);
        writer.writeF32(state.coinY);
        writer.writeU8(state.coinVisible ? 1u : 0u);
        writer.writeU32(state.level);
        writer.writeU32(state.eatCounter);
        writer.writeU32(state.lives);
        writer.writeU8(state.winning ? 1u : 0u);
    }

    bool readWorldSnapshotState(BufferReader &reader, WorldSnapshotState &state) {
        uint8_t coinVisible = 0;
        uint8_t winning = 0;
        if (!readSnakeSnapshotState(reader, state.localSnake) ||
            !readSnakeSnapshotState(reader, state.remoteSnake) ||
            !reader.readF32(state.coinX) ||
            !reader.readF32(state.coinY) ||
            !reader.readU8(coinVisible) ||
            !reader.readU32(state.level) ||
            !reader.readU32(state.eatCounter) ||
            !reader.readU32(state.lives) ||
            !reader.readU8(winning)) {
            return false;
        }

        state.coinVisible = coinVisible != 0;
        state.winning = winning != 0;
        return true;
    }
} // Net
