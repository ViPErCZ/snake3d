#ifndef SNAKE3_NETDISPATCHER_H
#define SNAKE3_NETDISPATCHER_H

#include <optional>
#include <variant>

#include <snake3d/Network/NetMessages.h>
#include <snake3d/Network/NetProtocol.h>

namespace Net {
    using NetMessage = std::variant<
        HelloMsg,
        WelcomeMsg,
        InputMsg,
        SnapshotMsg,
        PingMsg,
        PongMsg
    >;

    class NetDispatcher final {
    public:
        static std::optional<NetMessage> decode(const PacketView &packet);
    };
} // Net

#endif // SNAKE3_NETDISPATCHER_H
