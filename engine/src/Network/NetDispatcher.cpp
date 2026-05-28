#include <snake3d/Network/NetDispatcher.h>

namespace Net {
    std::optional<NetMessage> NetDispatcher::decode(const PacketView &packet) {
        switch (packet.type) {
            case MessageType::Hello: {
                HelloMsg msg{};
                if (!decodeHello(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            case MessageType::Welcome: {
                WelcomeMsg msg{};
                if (!decodeWelcome(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            case MessageType::Input: {
                InputMsg msg{};
                if (!decodeInput(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            case MessageType::Snapshot: {
                SnapshotMsg msg{};
                if (!decodeSnapshot(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            case MessageType::Ping: {
                PingMsg msg{};
                if (!decodePing(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            case MessageType::Pong: {
                PongMsg msg{};
                if (!decodePong(packet.payload, packet.payloadSize, msg)) {
                    return std::nullopt;
                }
                return msg;
            }
            default:
                break;
        }
        return std::nullopt;
    }
} // Net
