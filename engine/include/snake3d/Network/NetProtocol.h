#ifndef SNAKE3_NETPROTOCOL_H
#define SNAKE3_NETPROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

namespace Net {
    enum class MessageType : uint8_t {
        Hello = 1,
        Welcome = 2,
        Input = 3,
        Snapshot = 4,
        Ping = 5,
        Pong = 6,
        Disconnect = 7
    };

    struct PacketView {
        MessageType type = MessageType::Hello;
        const uint8_t *payload = nullptr;
        size_t payloadSize = 0;
    };

    class BufferWriter final {
    public:
        explicit BufferWriter(size_t reserve = 64);

        void writeU8(uint8_t v);
        void writeU16(uint16_t v);
        void writeU32(uint32_t v);
        void writeF32(float v);
        void writeBytes(const void *data, size_t size);
        void writeString(const std::string &value);

        [[nodiscard]] const std::vector<uint8_t> &data() const;
        void clear();

    private:
        std::vector<uint8_t> buffer;
    };

    class BufferReader final {
    public:
        BufferReader(const uint8_t *data, size_t size);

        bool readU8(uint8_t &v);
        bool readU16(uint16_t &v);
        bool readU32(uint32_t &v);
        bool readF32(float &v);
        bool readBytes(void *out, size_t sizeBytes);
        bool readString(std::string &value);

        [[nodiscard]] size_t remaining() const;

    private:
        const uint8_t *data = nullptr;
        size_t size = 0;
        size_t offset = 0;
    };

    std::vector<uint8_t> buildPacket(MessageType type, const std::vector<uint8_t> &payload);
    bool parsePacket(const uint8_t *data, size_t size, PacketView &out);
} // Net

#endif // SNAKE3_NETPROTOCOL_H
