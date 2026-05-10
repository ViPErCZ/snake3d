#include <catch2/catch_test_macros.hpp>

#include "../Network/NetMessages.h"
#include "../Network/NetProtocol.h"
#include "../Network/NetDispatcher.h"

using namespace Net;

TEST_CASE("Net protocol build/parse roundtrip", "[net]") {
    HelloMsg hello{};
    hello.protocolVersion = 42;
    hello.appVersion = 7;
    hello.name = "Tester";

    const auto payload = encodeHello(hello);
    const auto packet = buildPacket(MessageType::Hello, payload);

    PacketView view{};
    REQUIRE(parsePacket(packet.data(), packet.size(), view));
    REQUIRE(view.type == MessageType::Hello);

    HelloMsg decoded{};
    REQUIRE(decodeHello(view.payload, view.payloadSize, decoded));
    REQUIRE(decoded.protocolVersion == hello.protocolVersion);
    REQUIRE(decoded.appVersion == hello.appVersion);
    REQUIRE(decoded.name == hello.name);
}

TEST_CASE("Net dispatcher decodes input", "[net]") {
    InputMsg input{};
    input.tick = 123;
    input.moveX = -1;
    input.moveY = 1;
    input.actions = 3;

    const auto payload = encodeInput(input);
    const auto packet = buildPacket(MessageType::Input, payload);

    PacketView view{};
    REQUIRE(parsePacket(packet.data(), packet.size(), view));

    const auto decoded = NetDispatcher::decode(view);
    REQUIRE(decoded.has_value());
    REQUIRE(std::holds_alternative<InputMsg>(*decoded));
    const auto &msg = std::get<InputMsg>(*decoded);
    REQUIRE(msg.tick == input.tick);
    REQUIRE(msg.moveX == input.moveX);
    REQUIRE(msg.moveY == input.moveY);
    REQUIRE(msg.actions == input.actions);
}

TEST_CASE("Net snapshot encode/decode", "[net]") {
    SnapshotMsg snap{};
    snap.tick = 99;
    snap.payload = {1, 2, 3, 4, 5};

    const auto payload = encodeSnapshot(snap);
    SnapshotMsg decoded{};
    REQUIRE(decodeSnapshot(payload.data(), payload.size(), decoded));
    REQUIRE(decoded.tick == snap.tick);
    REQUIRE(decoded.payload == snap.payload);
}
