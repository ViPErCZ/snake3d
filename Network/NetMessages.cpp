#include "NetMessages.h"

namespace Net {
    std::vector<uint8_t> encodeHello(const HelloMsg &msg) {
        BufferWriter writer(64);
        writer.writeU32(msg.protocolVersion);
        writer.writeU32(msg.appVersion);
        writer.writeString(msg.name);
        return writer.data();
    }

    bool decodeHello(const uint8_t *data, const size_t size, HelloMsg &out) {
        BufferReader reader(data, size);
        if (!reader.readU32(out.protocolVersion)) return false;
        if (!reader.readU32(out.appVersion)) return false;
        if (!reader.readString(out.name)) return false;
        return true;
    }

    std::vector<uint8_t> encodeWelcome(const WelcomeMsg &msg) {
        BufferWriter writer(32);
        writer.writeU32(msg.protocolVersion);
        writer.writeU32(msg.assignedPeerId);
        writer.writeU32(msg.serverTick);
        writer.writeU32(msg.seed);
        return writer.data();
    }

    bool decodeWelcome(const uint8_t *data, const size_t size, WelcomeMsg &out) {
        BufferReader reader(data, size);
        if (!reader.readU32(out.protocolVersion)) return false;
        if (!reader.readU32(out.assignedPeerId)) return false;
        if (!reader.readU32(out.serverTick)) return false;
        if (!reader.readU32(out.seed)) return false;
        return true;
    }

    std::vector<uint8_t> encodeInput(const InputMsg &msg) {
        BufferWriter writer(16);
        writer.writeU32(msg.tick);
        writer.writeU8(static_cast<uint8_t>(msg.moveX));
        writer.writeU8(static_cast<uint8_t>(msg.moveY));
        writer.writeU8(msg.actions);
        return writer.data();
    }

    bool decodeInput(const uint8_t *data, const size_t size, InputMsg &out) {
        BufferReader reader(data, size);
        uint8_t mx = 0;
        uint8_t my = 0;
        if (!reader.readU32(out.tick)) return false;
        if (!reader.readU8(mx)) return false;
        if (!reader.readU8(my)) return false;
        if (!reader.readU8(out.actions)) return false;
        out.moveX = static_cast<int8_t>(mx);
        out.moveY = static_cast<int8_t>(my);
        return true;
    }

    std::vector<uint8_t> encodePing(const PingMsg &msg) {
        BufferWriter writer(8);
        writer.writeU32(msg.timeMs);
        return writer.data();
    }

    bool decodePing(const uint8_t *data, const size_t size, PingMsg &out) {
        BufferReader reader(data, size);
        return reader.readU32(out.timeMs);
    }

    std::vector<uint8_t> encodePong(const PongMsg &msg) {
        BufferWriter writer(8);
        writer.writeU32(msg.timeMs);
        return writer.data();
    }

    bool decodePong(const uint8_t *data, const size_t size, PongMsg &out) {
        BufferReader reader(data, size);
        return reader.readU32(out.timeMs);
    }

    std::vector<uint8_t> encodeSnapshot(const SnapshotMsg &msg) {
        BufferWriter writer(16 + msg.payload.size());
        writer.writeU32(msg.tick);
        const uint32_t size = msg.payload.size() > 0xFFFFFFFFu ? 0xFFFFFFFFu : static_cast<uint32_t>(msg.payload.size());
        writer.writeU32(size);
        if (size > 0) {
            writer.writeBytes(msg.payload.data(), size);
        }
        return writer.data();
    }

    bool decodeSnapshot(const uint8_t *data, const size_t size, SnapshotMsg &out) {
        BufferReader reader(data, size);
        uint32_t payloadSize = 0;
        if (!reader.readU32(out.tick)) return false;
        if (!reader.readU32(payloadSize)) return false;
        if (reader.remaining() < payloadSize) return false;
        out.payload.resize(payloadSize);
        if (payloadSize > 0) {
            if (!reader.readBytes(out.payload.data(), payloadSize)) return false;
        }
        return true;
    }
} // Net
