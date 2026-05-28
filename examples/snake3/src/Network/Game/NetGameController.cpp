#include "NetGameController.h"

#include <fstream>
#include <iostream>
#include <random>

#include <nlohmann/json.hpp>

#include "Network/NetDispatcher.h"
#include <snake3d/Tools/BuildSettings.h>

namespace Net {
    NetGameController::NetGameController()
        : netClient(manager), netServer(manager) {}

    void NetGameController::loadConfig(const std::string &path) {
        port = 7777;
        std::ifstream file(path);
        if (!file.is_open()) {
            return;
        }
        try {
            nlohmann::json j;
            file >> j;
            if (j.contains("network") && j["network"].contains("port")) {
                port = j["network"]["port"].get<uint16_t>();
            }
        } catch (...) {
            port = 7777;
        }
    }

    bool NetGameController::host() {
        if (enabled) {
            reset();
        }
        const bool ok = netServer.start(port);
        if (ok) {
            server = true;
            enabled = true;
            seed = static_cast<uint32_t>(std::random_device{}());
        }
        return ok;
    }

    bool NetGameController::join(const std::string &ip, const std::string &playerName) {
        if (enabled) {
            reset();
        }
        const bool ok = netClient.connect(ip, port, playerName);
        if (ok) {
            client = true;
            enabled = true;
        }
        return ok;
    }

    void NetGameController::notifyDisconnect() const {
        if (!enabled) {
            return;
        }
        if (server) {
            manager.disconnectAllNow(0);
        } else if (client) {
            manager.disconnectNow(0, 0);
        }
        manager.flush();
    }

    void NetGameController::reset() {
        notifyDisconnect();
        manager.shutdown();
        enabled = false;
        server = false;
        client = false;
        peerId = 0;
        seed = 0;
        clock.reset(0);
        localRespawnSerial = 0;
        remoteRespawnSerial = 0;
        netLastSeenLocalRespawnSerial = 0;
        netLastSeenRemoteRespawnSerial = 0;
        netLastSeenLocalCrash = false;
        netLastSeenRemoteCrash = false;
        netLastSnapshotLevel = 0;
        netLastSnapshotEatCounter = 0;
        pendingLocalRespawn = {};
    }

    void NetGameController::tick(NetWorldSource &source, NetWorldSink &sink) {
        if (!enabled) {
            return;
        }
        clock.advance(1);

        NetEvent event{};
        PacketView packet{};

        if (server) {
            while (true) {
                if (!server) {
                    break;
                }
                if (!netServer.poll(event, packet)) {
                    break;
                }
                handleServerEvent(event, packet, sink);
            }
            if (enabled && server) {
                broadcastWorldSnapshot(source);
            }
        } else if (client) {
            while (true) {
                if (!client) {
                    break;
                }
                if (!netClient.poll(event, packet)) {
                    break;
                }
                handleClientEvent(event, packet, sink);
            }
        }
    }

    bool NetGameController::sendInput(const InputMsg &msg) const {
        if (!client) {
            return false;
        }
        return netClient.sendInput(msg);
    }

    void NetGameController::sendPauseToggle() const {
        if (!enabled || !client) {
            return;
        }
        InputMsg input{};
        input.tick = clock.getTick();
        input.actions = 1;
        input.moveX = 0;
        input.moveY = 0;
        const bool sent = netClient.sendInput(input);
        if (!sent) {
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Failed to send pause toggle to server" << std::endl;
            }
        }
    }

    uint32_t NetGameController::bumpLocalRespawnSerial() {
        return ++localRespawnSerial;
    }

    uint32_t NetGameController::bumpRemoteRespawnSerial() {
        return ++remoteRespawnSerial;
    }

    void NetGameController::clearPendingLocalRespawn() {
        pendingLocalRespawn = {};
    }

    void NetGameController::handleServerEvent(NetEvent &event, PacketView &packet, NetWorldSink &sink) {
        if (event.type == NetEventType::Connect) {
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Client connected: " << event.peerId << std::endl;
            }
            return;
        }
        if (event.type == NetEventType::Disconnect) {
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Client disconnected: " << event.peerId << std::endl;
            }
            sink.onPeerDisconnected();
            return;
        }
        if (event.type != NetEventType::Receive) {
            return;
        }

        const auto decoded = NetDispatcher::decode(packet);
        if (!decoded.has_value()) {
            return;
        }

        if (auto *hello = std::get_if<HelloMsg>(&decoded.value())) {
            WelcomeMsg welcome{};
            welcome.protocolVersion = Protocol::kProtocolVersion;
            welcome.assignedPeerId = event.peerId;
            welcome.serverTick = clock.getTick();
            welcome.seed = seed;
            (void) netServer.sendWelcome(event.peerId, welcome);
            sink.onClientHello(event.peerId);
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Hello from peer " << event.peerId << " name=" << hello->name << std::endl;
            }
        } else if (auto *input = std::get_if<InputMsg>(&decoded.value())) {
            sink.onRemoteInput(input->moveX, input->moveY, input->actions);
        } else if (auto *ping = std::get_if<PingMsg>(&decoded.value())) {
            PongMsg pong{};
            pong.timeMs = ping->timeMs;
            (void) netServer.sendPong(event.peerId, pong);
        }
    }

    void NetGameController::handleClientEvent(NetEvent &event, PacketView &packet, NetWorldSink &sink) {
        if (event.type == NetEventType::Disconnect) {
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Disconnected from server" << std::endl;
            }
            sink.onPeerDisconnected();
            return;
        }
        if (event.type != NetEventType::Receive) {
            return;
        }

        const auto decoded = NetDispatcher::decode(packet);
        if (!decoded.has_value()) {
            return;
        }

        if (auto *welcome = std::get_if<WelcomeMsg>(&decoded.value())) {
            peerId = welcome->assignedPeerId;
            seed = welcome->seed;
            sink.onWelcomeReceived();
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Welcome: peerId=" << peerId << " seed=" << seed << std::endl;
            }
        } else if (auto *ping = std::get_if<PingMsg>(&decoded.value())) {
            PongMsg pong{};
            pong.timeMs = ping->timeMs;
            (void) netClient.sendPong(pong);
        } else if (auto *pong = std::get_if<PongMsg>(&decoded.value())) {
            if constexpr (Build::isDebug) {
                std::cout << "[Net] Pong: " << pong->timeMs << std::endl;
            }
        } else if (auto *snap = std::get_if<SnapshotMsg>(&decoded.value())) {
            applyClientSnapshot(*snap, sink);
        }
    }

    void NetGameController::broadcastWorldSnapshot(NetWorldSource &source) {
        WorldSnapshotState snapshot{};
        if (!source.collectWorldSnapshot(snapshot)) {
            return;
        }
        snapshot.localSnake.respawnSerial = localRespawnSerial;
        snapshot.remoteSnake.respawnSerial = remoteRespawnSerial;
        BufferWriter writer(4096);
        writeWorldSnapshotState(writer, snapshot);
        SnapshotMsg snap{};
        snap.tick = clock.getTick();
        snap.payload = writer.data();
        netServer.broadcastSnapshot(snap);
    }

    void NetGameController::applyClientSnapshot(const SnapshotMsg &snap, NetWorldSink &sink) {
        if (snap.payload.size() < 4) {
            return;
        }
        BufferReader reader(snap.payload.data(), snap.payload.size());
        WorldSnapshotState snapshot{};
        if (!readWorldSnapshotState(reader, snapshot)) {
            return;
        }

        const auto &authoritative = snapshot.localSnake;
        const auto &clientSnake = snapshot.remoteSnake;

        if (!snapshot.winning) {
            if (clientSnake.positions.empty()) {
                return;
            }
            const bool localRespawnDetected =
                clientSnake.respawnSerial != netLastSeenLocalRespawnSerial;
            const bool localCrashActive = clientSnake.crashActive;
            const bool remoteCrashActive = authoritative.crashActive;
            netLastSeenLocalRespawnSerial = clientSnake.respawnSerial;
            netLastSeenRemoteRespawnSerial = authoritative.respawnSerial;

            if (localRespawnDetected && !pendingLocalRespawn.active) {
                if constexpr (Build::isDebug) {
                    std::cout << "[Net][ClientRespawnDetected] serial=" << clientSnake.respawnSerial
                              << " dir=" << static_cast<int>(clientSnake.direction)
                              << " count=" << clientSnake.segmentCount
                              << " positions=" << clientSnake.positions.size() << std::endl;
                }
                sink.scheduleLocalRespawnAfterCrash(clientSnake.positions, clientSnake.direction);
                sink.requestLocalCrash();
                pendingLocalRespawn.positions = clientSnake.positions;
                pendingLocalRespawn.segmentCount = clientSnake.segmentCount;
                pendingLocalRespawn.direction = clientSnake.direction;
                pendingLocalRespawn.active = true;
            } else if (localCrashActive) {
                if (!netLastSeenLocalCrash) {
                    sink.requestLocalCrash();
                }
            } else if (!pendingLocalRespawn.active) {
                sink.applyLocalSnakePositions(clientSnake);
            }
            if (remoteCrashActive && !netLastSeenRemoteCrash) {
                sink.requestRemoteCrash();
            }
            sink.applyRemoteSnakePositions(authoritative);
            netLastSeenLocalCrash = localCrashActive;
            netLastSeenRemoteCrash = remoteCrashActive;
        }

        const bool coinAdvanced = (snapshot.level == netLastSnapshotLevel &&
                                   snapshot.eatCounter != netLastSnapshotEatCounter);
        sink.applyCoin(snapshot.coinX, snapshot.coinY, snapshot.coinVisible,
                       coinAdvanced && snapshot.coinVisible);
        sink.applyHud(snapshot.level, snapshot.eatCounter, snapshot.lives);

        if (snapshot.winning) {
            sink.applyWinning();
        }

        netLastSnapshotLevel = snapshot.level;
        netLastSnapshotEatCounter = snapshot.eatCounter;
    }
}
