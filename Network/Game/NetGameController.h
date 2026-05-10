#ifndef SNAKE3_NETGAMECONTROLLER_H
#define SNAKE3_NETGAMECONTROLLER_H

#include <cstdint>
#include <string>
#include <vector>

#include "../NetClientServer.h"
#include "../NetClock.h"
#include "../NetManager.h"
#include "../NetMessages.h"
#include "../NetProtocol.h"
#include "NetGameSnapshot.h"

namespace Net {
    struct PendingRespawnState {
        std::vector<glm::vec2> positions;
        size_t segmentCount = 0;
        SnakeMeshNode3D::eDIRECTION direction = SnakeMeshNode3D::NONE;
        bool active = false;
    };

    class NetWorldSource {
    public:
        virtual ~NetWorldSource() = default;
        virtual bool collectWorldSnapshot(WorldSnapshotState &out) const = 0;
    };

    class NetWorldSink {
    public:
        virtual ~NetWorldSink() = default;

        virtual void onClientHello(uint32_t peerId) = 0;
        virtual void onWelcomeReceived() = 0;
        virtual void onPeerDisconnected() = 0;
        virtual void onRemoteInput(int8_t moveX, int8_t moveY, uint8_t actions) = 0;

        virtual void requestLocalCrash() = 0;
        virtual void requestRemoteCrash() = 0;
        virtual void scheduleLocalRespawnAfterCrash(const std::vector<glm::vec2> &positions,
                                                    SnakeMeshNode3D::eDIRECTION direction) = 0;
        virtual void applyLocalSnakePositions(const SnakeSnapshotState &snake) = 0;
        virtual void applyRemoteSnakePositions(const SnakeSnapshotState &snake) = 0;

        virtual void applyCoin(float x, float y, bool visible, bool eatenAnim) = 0;
        virtual void applyHud(uint32_t level, uint32_t eatCounter, uint32_t lives) = 0;
        virtual void applyWinning() = 0;
    };

    class NetGameController final {
    public:
        NetGameController();

        void loadConfig(const std::string &path = "Assets/config.json");

        bool host();
        bool join(const std::string &ip, const std::string &playerName);
        void notifyDisconnect() const;
        void reset();

        void tick(NetWorldSource &source, NetWorldSink &sink);

        bool sendInput(const InputMsg &msg) const;
        void sendPauseToggle() const;

        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] bool isServer() const { return server; }
        [[nodiscard]] bool isClient() const { return client; }
        [[nodiscard]] uint16_t getPort() const { return port; }
        [[nodiscard]] uint32_t getPeerId() const { return peerId; }
        [[nodiscard]] uint32_t getSeed() const { return seed; }
        [[nodiscard]] uint32_t getTick() const { return clock.getTick(); }

        uint32_t bumpLocalRespawnSerial();
        uint32_t bumpRemoteRespawnSerial();
        [[nodiscard]] bool hasPendingLocalRespawn() const { return pendingLocalRespawn.active; }
        [[nodiscard]] const PendingRespawnState &getPendingLocalRespawn() const { return pendingLocalRespawn; }
        void clearPendingLocalRespawn();

    private:
        void handleServerEvent(NetEvent &event, PacketView &packet, NetWorldSink &sink);
        void handleClientEvent(NetEvent &event, PacketView &packet, NetWorldSink &sink);
        void broadcastWorldSnapshot(NetWorldSource &source);
        void applyClientSnapshot(const SnapshotMsg &snap, NetWorldSink &sink);

        NetManager manager;
        NetClient netClient;
        NetServer netServer;
        NetClock clock{60};
        uint16_t port = 7777;
        bool enabled = false;
        bool server = false;
        bool client = false;
        uint32_t peerId = 0;
        uint32_t seed = 0;

        uint32_t localRespawnSerial = 0;
        uint32_t remoteRespawnSerial = 0;
        uint32_t netLastSeenLocalRespawnSerial = 0;
        uint32_t netLastSeenRemoteRespawnSerial = 0;
        bool netLastSeenLocalCrash = false;
        bool netLastSeenRemoteCrash = false;
        uint32_t netLastSnapshotLevel = 0;
        uint32_t netLastSnapshotEatCounter = 0;
        PendingRespawnState pendingLocalRespawn;
    };
}

#endif //SNAKE3_NETGAMECONTROLLER_H
