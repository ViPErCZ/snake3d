#include <snake3d/Network/NetProtocol.h>

#include <cstring>

namespace Net {
    BufferWriter::BufferWriter(const size_t reserve) {
        buffer.reserve(reserve);
    }

    void BufferWriter::writeU8(const uint8_t v) {
        buffer.push_back(v);
    }

    void BufferWriter::writeU16(const uint16_t v) {
        buffer.push_back(static_cast<uint8_t>(v & 0xFF));
        buffer.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    }

    void BufferWriter::writeU32(const uint32_t v) {
        buffer.push_back(static_cast<uint8_t>(v & 0xFF));
        buffer.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    }

    void BufferWriter::writeF32(const float v) {
        uint32_t raw;
        static_assert(sizeof(float) == sizeof(uint32_t));
        std::memcpy(&raw, &v, sizeof(uint32_t));
        writeU32(raw);
    }

    void BufferWriter::writeBytes(const void *data, const size_t size) {
        if (!data || size == 0) {
            return;
        }
        const auto *ptr = static_cast<const uint8_t *>(data);
        buffer.insert(buffer.end(), ptr, ptr + size);
    }

    void BufferWriter::writeString(const std::string &value) {
        if (value.size() > 65535) {
            writeU16(0);
            return;
        }
        writeU16(static_cast<uint16_t>(value.size()));
        writeBytes(value.data(), value.size());
    }

    const std::vector<uint8_t> &BufferWriter::data() const {
        return buffer;
    }

    void BufferWriter::clear() {
        buffer.clear();
    }

    BufferReader::BufferReader(const uint8_t *data, const size_t size)
        : data(data), size(size) {}

    bool BufferReader::readU8(uint8_t &v) {
        if (offset + 1 > size) {
            return false;
        }
        v = data[offset];
        offset += 1;
        return true;
    }

    bool BufferReader::readU16(uint16_t &v) {
        if (offset + 2 > size) {
            return false;
        }
        v = static_cast<uint16_t>(data[offset]) |
            (static_cast<uint16_t>(data[offset + 1]) << 8);
        offset += 2;
        return true;
    }

    bool BufferReader::readU32(uint32_t &v) {
        if (offset + 4 > size) {
            return false;
        }
        v = static_cast<uint32_t>(data[offset]) |
            (static_cast<uint32_t>(data[offset + 1]) << 8) |
            (static_cast<uint32_t>(data[offset + 2]) << 16) |
            (static_cast<uint32_t>(data[offset + 3]) << 24);
        offset += 4;
        return true;
    }

    bool BufferReader::readF32(float &v) {
        uint32_t raw;
        if (!readU32(raw)) {
            return false;
        }
        std::memcpy(&v, &raw, sizeof(uint32_t));
        return true;
    }

    bool BufferReader::readBytes(void *out, const size_t sizeBytes) {
        if (!out || sizeBytes == 0) {
            return false;
        }
        if (offset + sizeBytes > size) {
            return false;
        }
        std::memcpy(out, data + offset, sizeBytes);
        offset += sizeBytes;
        return true;
    }

    bool BufferReader::readString(std::string &value) {
        uint16_t len = 0;
        if (!readU16(len)) {
            return false;
        }
        if (offset + len > size) {
            return false;
        }
        value.assign(reinterpret_cast<const char *>(data + offset), len);
        offset += len;
        return true;
    }

    size_t BufferReader::remaining() const {
        return (offset <= size) ? (size - offset) : 0;
    }

    std::vector<uint8_t> buildPacket(const MessageType type, const std::vector<uint8_t> &payload) {
        std::vector<uint8_t> out;
        const uint16_t payloadSize = payload.size() > 65535 ? 65535 : static_cast<uint16_t>(payload.size());
        out.reserve(1 + 2 + payloadSize);
        out.push_back(static_cast<uint8_t>(type));
        out.push_back(static_cast<uint8_t>(payloadSize & 0xFF));
        out.push_back(static_cast<uint8_t>((payloadSize >> 8) & 0xFF));
        out.insert(out.end(), payload.begin(), payload.begin() + payloadSize);
        return out;
    }

    bool parsePacket(const uint8_t *data, const size_t size, PacketView &out) {
        if (!data || size < 3) {
            return false;
        }
        out.type = static_cast<MessageType>(data[0]);
        const uint16_t payloadSize = static_cast<uint16_t>(data[1]) |
                                     (static_cast<uint16_t>(data[2]) << 8);
        if (3 + payloadSize > size) {
            return false;
        }
        out.payload = data + 3;
        out.payloadSize = payloadSize;
        return true;
    }
} // Net
