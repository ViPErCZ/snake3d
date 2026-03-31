#ifndef SNAKE3_NETMESSAGES_H
#define SNAKE3_NETMESSAGES_H

#include <cstdint>
#include <string>
#include <vector>

#include "NetProtocol.h"

namespace Net {
    namespace Protocol {
        constexpr uint32_t kProtocolVersion = 1;
    }

    struct HelloMsg {
        uint32_t protocolVersion = Protocol::kProtocolVersion;
        uint32_t appVersion = 1;
        std::string name{};
    };

    struct WelcomeMsg {
        uint32_t protocolVersion = Protocol::kProtocolVersion;
        uint32_t assignedPeerId = 0;
        uint32_t serverTick = 0;
        uint32_t seed = 0;
    };

    struct InputMsg {
        uint32_t tick = 0;
        int8_t moveX = 0;
        int8_t moveY = 0;
        uint8_t actions = 0;
    };

    struct PingMsg {
        uint32_t timeMs = 0;
    };

    struct PongMsg {
        uint32_t timeMs = 0;
    };

    struct SnapshotMsg {
        uint32_t tick = 0;
        std::vector<uint8_t> payload{};
    };

    std::vector<uint8_t> encodeHello(const HelloMsg &msg);
    bool decodeHello(const uint8_t *data, size_t size, HelloMsg &out);

    std::vector<uint8_t> encodeWelcome(const WelcomeMsg &msg);
    bool decodeWelcome(const uint8_t *data, size_t size, WelcomeMsg &out);

    std::vector<uint8_t> encodeInput(const InputMsg &msg);
    bool decodeInput(const uint8_t *data, size_t size, InputMsg &out);

    std::vector<uint8_t> encodePing(const PingMsg &msg);
    bool decodePing(const uint8_t *data, size_t size, PingMsg &out);

    std::vector<uint8_t> encodePong(const PongMsg &msg);
    bool decodePong(const uint8_t *data, size_t size, PongMsg &out);

    std::vector<uint8_t> encodeSnapshot(const SnapshotMsg &msg);
    bool decodeSnapshot(const uint8_t *data, size_t size, SnapshotMsg &out);
} // Net

#endif // SNAKE3_NETMESSAGES_H
