#include <snake3d/Network/NetManager.h>

#include <cstring>

#ifdef SNAKE3_USE_ENET
#include <enet/enet.h>
#endif

namespace Net {
    struct NetManager::Impl {
#ifdef SNAKE3_USE_ENET
        ENetHost *host = nullptr;
        ENetPeer *serverPeer = nullptr;
#endif
        NetMode mode = NetMode::None;
        bool initialized = false;
        size_t peerCount = 0;
    };

    NetManager::NetManager() : impl(new Impl()) {}

    NetManager::~NetManager() {
        shutdown();
        delete impl;
        impl = nullptr;
    }

    bool NetManager::initialize() const {
        if (impl->initialized) {
            return true;
        }
#ifdef SNAKE3_USE_ENET
        if (enet_initialize() != 0) {
            return false;
        }
        impl->initialized = true;
        return true;
#else
        return false;
#endif
    }

    void NetManager::shutdown() const {
#ifdef SNAKE3_USE_ENET
        if (impl->host) {
            enet_host_destroy(impl->host);
            impl->host = nullptr;
        }
        impl->serverPeer = nullptr;
        if (impl->initialized) {
            enet_deinitialize();
        }
#endif
        impl->initialized = false;
        impl->mode = NetMode::None;
        impl->peerCount = 0;
    }

    bool NetManager::createServer(const uint16_t port, const size_t maxPeers, const size_t channels,
                                  const uint32_t inBandwidth, const uint32_t outBandwidth) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->initialized) {
            if (!initialize()) {
                return false;
            }
        }
        if (impl->host) {
            return false;
        }

        ENetAddress address{};
        address.host = ENET_HOST_ANY;
        address.port = port;
        impl->host = enet_host_create(&address, maxPeers, channels, inBandwidth, outBandwidth);
        if (!impl->host) {
            return false;
        }
        impl->mode = NetMode::Server;
        return true;
#else
        (void)port; (void)maxPeers; (void)channels; (void)inBandwidth; (void)outBandwidth;
        return false;
#endif
    }

    bool NetManager::createClient(const size_t channels, const uint32_t inBandwidth, const uint32_t outBandwidth) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->initialized) {
            if (!initialize()) {
                return false;
            }
        }
        if (impl->host) {
            return false;
        }

        impl->host = enet_host_create(nullptr, 1, channels, inBandwidth, outBandwidth);
        if (!impl->host) {
            return false;
        }
        impl->mode = NetMode::Client;
        return true;
#else
        (void)channels; (void)inBandwidth; (void)outBandwidth;
        return false;
#endif
    }

    bool NetManager::connect(const std::string &host, const uint16_t port, const uint32_t timeoutMs) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host || impl->mode != NetMode::Client) {
            return false;
        }
        ENetAddress address{};
        if (enet_address_set_host(&address, host.c_str()) != 0) {
            return false;
        }
        address.port = port;

        impl->serverPeer = enet_host_connect(impl->host, &address, 2, 0);
        if (!impl->serverPeer) {
            return false;
        }

        if (timeoutMs == 0) {
            return true;
        }

        ENetEvent event;
        if (enet_host_service(impl->host, &event, timeoutMs) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {
            return true;
        }
        enet_peer_reset(impl->serverPeer);
        impl->serverPeer = nullptr;
        return false;
#else
        (void)host; (void)port; (void)timeoutMs;
        return false;
#endif
    }

    void NetManager::disconnect(const uint32_t peerId, const uint32_t data) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host) {
            return;
        }
        if (impl->mode == NetMode::Client) {
            if (impl->serverPeer) {
                enet_peer_disconnect(impl->serverPeer, data);
            }
            return;
        }

        for (size_t i = 0; i < impl->host->peerCount; ++i) {
            ENetPeer *peer = &impl->host->peers[i];
            if (peer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }
            if (peer->incomingPeerID == peerId) {
                enet_peer_disconnect(peer, data);
                return;
            }
        }
#else
        (void)peerId; (void)data;
#endif
    }

    void NetManager::disconnectAll(const uint32_t data) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host) {
            return;
        }
        for (size_t i = 0; i < impl->host->peerCount; ++i) {
            ENetPeer *peer = &impl->host->peers[i];
            if (peer->state == ENET_PEER_STATE_CONNECTED) {
                enet_peer_disconnect(peer, data);
            }
        }
#else
        (void)data;
#endif
    }

    void NetManager::disconnectNow(const uint32_t peerId, const uint32_t data) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host) {
            return;
        }
        if (impl->mode == NetMode::Client) {
            if (impl->serverPeer) {
                enet_peer_disconnect_now(impl->serverPeer, data);
            }
            return;
        }

        for (size_t i = 0; i < impl->host->peerCount; ++i) {
            ENetPeer *peer = &impl->host->peers[i];
            if (peer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }
            if (peer->incomingPeerID == peerId) {
                enet_peer_disconnect_now(peer, data);
                return;
            }
        }
#else
        (void)peerId; (void)data;
#endif
    }

    void NetManager::disconnectAllNow(const uint32_t data) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host) {
            return;
        }
        for (size_t i = 0; i < impl->host->peerCount; ++i) {
            ENetPeer *peer = &impl->host->peers[i];
            if (peer->state == ENET_PEER_STATE_CONNECTED) {
                enet_peer_disconnect_now(peer, data);
            }
        }
#else
        (void)data;
#endif
    }

    bool NetManager::service(NetEvent &outEvent, const uint32_t timeoutMs) const {
        outEvent = NetEvent{};
#ifdef SNAKE3_USE_ENET
        if (!impl->host) {
            return false;
        }
        ENetEvent event;
        if (enet_host_service(impl->host, &event, timeoutMs) <= 0) {
            return false;
        }
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                outEvent.type = NetEventType::Connect;
                outEvent.peerId = event.peer->incomingPeerID;
                ++impl->peerCount;
                return true;
            case ENET_EVENT_TYPE_DISCONNECT:
                outEvent.type = NetEventType::Disconnect;
                outEvent.peerId = event.peer->incomingPeerID;
                if (impl->peerCount > 0) {
                    --impl->peerCount;
                }
                return true;
            case ENET_EVENT_TYPE_RECEIVE:
                outEvent.type = NetEventType::Receive;
                outEvent.peerId = event.peer->incomingPeerID;
                outEvent.channel = event.channelID;
                outEvent.payload.resize(event.packet->dataLength);
                std::memcpy(outEvent.payload.data(), event.packet->data, event.packet->dataLength);
                enet_packet_destroy(event.packet);
                return true;
            default:
                break;
        }
        return false;
#else
        (void)timeoutMs;
        return false;
#endif
    }

    bool NetManager::send(const uint32_t peerId, const uint8_t channel, const void *data,
                          const size_t size, const bool reliable) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host || !data || size == 0) {
            return false;
        }
        ENetPacket *packet = enet_packet_create(data, size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        if (!packet) {
            return false;
        }
        if (impl->mode == NetMode::Client) {
            if (!impl->serverPeer) {
                enet_packet_destroy(packet);
                return false;
            }
            if (enet_peer_send(impl->serverPeer, channel, packet) != 0) {
                enet_packet_destroy(packet);
                return false;
            }
            return true;
        }
        for (size_t i = 0; i < impl->host->peerCount; ++i) {
            ENetPeer *peer = &impl->host->peers[i];
            if (peer->state != ENET_PEER_STATE_CONNECTED) {
                continue;
            }
            if (peer->incomingPeerID == peerId) {
                if (enet_peer_send(peer, channel, packet) != 0) {
                    enet_packet_destroy(packet);
                    return false;
                }
                return true;
            }
        }
        enet_packet_destroy(packet);
        return false;
#else
        (void)peerId; (void)channel; (void)data; (void)size; (void)reliable;
        return false;
#endif
    }

    void NetManager::broadcast(const uint8_t channel, const void *data,
                               const size_t size, const bool reliable) const {
#ifdef SNAKE3_USE_ENET
        if (!impl->host || !data || size == 0) {
            return;
        }
        ENetPacket *packet = enet_packet_create(data, size,
            reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
        if (!packet) {
            return;
        }
        enet_host_broadcast(impl->host, channel, packet);
#else
        (void)channel; (void)data; (void)size; (void)reliable;
#endif
    }

    void NetManager::flush() const {
#ifdef SNAKE3_USE_ENET
        if (impl->host) {
            enet_host_flush(impl->host);
        }
#endif
    }

    NetMode NetManager::getMode() const {
        return impl->mode;
    }

    bool NetManager::isActive() const {
#ifdef SNAKE3_USE_ENET
        return impl->host != nullptr;
#else
        return false;
#endif
    }

    size_t NetManager::getPeerCount() const {
        return impl->peerCount;
    }
} // Net
