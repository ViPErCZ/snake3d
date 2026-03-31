#include "NetClientServer.h"

namespace Net {
    namespace {
        constexpr uint8_t kDefaultControlChannel = 0;
        constexpr uint8_t kDefaultUnreliableChannel = 1;
    }

    NetClient::NetClient(NetManager &manager) : net(manager),
        controlChannel(kDefaultControlChannel),
        unreliableChannel(kDefaultUnreliableChannel) {}

    void NetClient::setChannels(const uint8_t controlChannelIn, const uint8_t unreliableChannelIn) {
        controlChannel = controlChannelIn;
        unreliableChannel = unreliableChannelIn;
    }

    bool NetClient::connect(const std::string &host, const uint16_t port, const std::string &name) const {
        if (!net.createClient()) {
            return false;
        }
        if (!net.connect(host, port)) {
            return false;
        }
        HelloMsg hello{};
        hello.name = name;
        return sendHello(hello);
    }

    bool NetClient::poll(NetEvent &event, PacketView &packet) const {
        if (!net.service(event, 0)) {
            return false;
        }
        if (event.type != NetEventType::Receive) {
            return true;
        }
        return parsePacket(event.payload.data(), event.payload.size(), packet);
    }

    bool NetClient::sendHello(const HelloMsg &msg) const {
        const auto payload = encodeHello(msg);
        const auto packet = buildPacket(MessageType::Hello, payload);
        return net.send(0, controlChannel, packet.data(), packet.size(), true);
    }

    bool NetClient::sendInput(const InputMsg &msg) const {
        const auto payload = encodeInput(msg);
        const auto packet = buildPacket(MessageType::Input, payload);
        return net.send(0, unreliableChannel, packet.data(), packet.size(), false);
    }

    bool NetClient::sendPing(const PingMsg &msg) const {
        const auto payload = encodePing(msg);
        const auto packet = buildPacket(MessageType::Ping, payload);
        return net.send(0, controlChannel, packet.data(), packet.size(), true);
    }

    bool NetClient::sendPong(const PongMsg &msg) const {
        const auto payload = encodePong(msg);
        const auto packet = buildPacket(MessageType::Pong, payload);
        return net.send(0, controlChannel, packet.data(), packet.size(), true);
    }

    NetServer::NetServer(NetManager &manager) : net(manager),
        controlChannel(kDefaultControlChannel),
        unreliableChannel(kDefaultUnreliableChannel) {}

    void NetServer::setChannels(const uint8_t controlChannelIn, const uint8_t unreliableChannelIn) {
        controlChannel = controlChannelIn;
        unreliableChannel = unreliableChannelIn;
    }

    bool NetServer::start(const uint16_t port, const size_t maxPeers) const {
        return net.createServer(port, maxPeers);
    }

    bool NetServer::poll(NetEvent &event, PacketView &packet) const {
        if (!net.service(event, 0)) {
            return false;
        }
        if (event.type != NetEventType::Receive) {
            return true;
        }
        return parsePacket(event.payload.data(), event.payload.size(), packet);
    }

    bool NetServer::sendWelcome(const uint32_t peerId, const WelcomeMsg &msg) const {
        const auto payload = encodeWelcome(msg);
        const auto packet = buildPacket(MessageType::Welcome, payload);
        return net.send(peerId, controlChannel, packet.data(), packet.size(), true);
    }

    bool NetServer::sendSnapshot(const uint32_t peerId, const SnapshotMsg &msg) const {
        const auto payload = encodeSnapshot(msg);
        const auto packet = buildPacket(MessageType::Snapshot, payload);
        return net.send(peerId, unreliableChannel, packet.data(), packet.size(), false);
    }

    void NetServer::broadcastSnapshot(const SnapshotMsg &msg) const {
        const auto payload = encodeSnapshot(msg);
        const auto packet = buildPacket(MessageType::Snapshot, payload);
        net.broadcast(unreliableChannel, packet.data(), packet.size(), false);
    }

    bool NetServer::sendPing(const uint32_t peerId, const PingMsg &msg) const {
        const auto payload = encodePing(msg);
        const auto packet = buildPacket(MessageType::Ping, payload);
        return net.send(peerId, controlChannel, packet.data(), packet.size(), true);
    }

    bool NetServer::sendPong(const uint32_t peerId, const PongMsg &msg) const {
        const auto payload = encodePong(msg);
        const auto packet = buildPacket(MessageType::Pong, payload);
        return net.send(peerId, controlChannel, packet.data(), packet.size(), true);
    }
} // Net
