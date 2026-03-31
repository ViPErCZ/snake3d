#ifndef SNAKE3_NETCLIENTSERVER_H
#define SNAKE3_NETCLIENTSERVER_H

#include <cstdint>
#include <string>
#include <vector>

#include "NetManager.h"
#include "NetMessages.h"
#include "NetProtocol.h"

namespace Net {
    class NetClient final {
    public:
        explicit NetClient(NetManager &manager);

        void setChannels(uint8_t controlChannelIn, uint8_t unreliableChannelIn);

        [[nodiscard]] bool connect(const std::string &host, uint16_t port, const std::string &name) const;
        bool poll(NetEvent &event, PacketView &packet) const;

        [[nodiscard]] bool sendHello(const HelloMsg &msg) const;
        [[nodiscard]] bool sendInput(const InputMsg &msg) const;
        [[nodiscard]] bool sendPing(const PingMsg &msg) const;
        [[nodiscard]] bool sendPong(const PongMsg &msg) const;

    private:
        NetManager &net;
        uint8_t controlChannel = 0;
        uint8_t unreliableChannel = 1;
    };

    class NetServer final {
    public:
        explicit NetServer(NetManager &manager);

        void setChannels(uint8_t controlChannelIn, uint8_t unreliableChannelIn);

        [[nodiscard]] bool start(uint16_t port, size_t maxPeers = 32) const;
        bool poll(NetEvent &event, PacketView &packet) const;

        [[nodiscard]] bool sendWelcome(uint32_t peerId, const WelcomeMsg &msg) const;
        [[nodiscard]] bool sendSnapshot(uint32_t peerId, const SnapshotMsg &msg) const;
        void broadcastSnapshot(const SnapshotMsg &msg) const;
        [[nodiscard]] bool sendPing(uint32_t peerId, const PingMsg &msg) const;
        [[nodiscard]] bool sendPong(uint32_t peerId, const PongMsg &msg) const;

    private:
        NetManager &net;
        uint8_t controlChannel = 0;
        uint8_t unreliableChannel = 1;
    };
} // Net

#endif // SNAKE3_NETCLIENTSERVER_H
