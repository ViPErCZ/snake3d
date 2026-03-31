#ifndef SNAKE3_NETMANAGER_H
#define SNAKE3_NETMANAGER_H

#include <cstdint>
#include <string>
#include <vector>

namespace Net {
    enum class NetMode {
        None,
        Server,
        Client
    };

    enum class NetEventType {
        None,
        Connect,
        Disconnect,
        Receive
    };

    struct NetEvent {
        NetEventType type = NetEventType::None;
        uint32_t peerId = 0;
        uint8_t channel = 0;
        std::vector<uint8_t> payload{};
    };

    class NetManager final {
    public:
        NetManager();
        ~NetManager();

        [[nodiscard]] bool initialize() const;
        void shutdown() const;

        [[nodiscard]] bool createServer(uint16_t port, size_t maxPeers = 32, size_t channels = 2,
                          uint32_t inBandwidth = 0, uint32_t outBandwidth = 0) const;
        [[nodiscard]] bool createClient(size_t channels = 2, uint32_t inBandwidth = 0, uint32_t outBandwidth = 0) const;
        [[nodiscard]] bool connect(const std::string &host, uint16_t port, uint32_t timeoutMs = 5000) const;

        void disconnect(uint32_t peerId, uint32_t data = 0) const;
        void disconnectAll(uint32_t data = 0) const;
        void disconnectNow(uint32_t peerId, uint32_t data = 0) const;
        void disconnectAllNow(uint32_t data = 0) const;

        bool service(NetEvent &outEvent, uint32_t timeoutMs = 0) const;

        bool send(uint32_t peerId, uint8_t channel, const void *data, size_t size, bool reliable) const;
        void broadcast(uint8_t channel, const void *data, size_t size, bool reliable) const;
        void flush() const;

        [[nodiscard]] NetMode getMode() const;
        [[nodiscard]] bool isActive() const;
        [[nodiscard]] size_t getPeerCount() const;

    private:
        struct Impl;
        Impl *impl = nullptr;
    };
} // Net

#endif // SNAKE3_NETMANAGER_H
