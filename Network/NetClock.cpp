#include "NetClock.h"

#include <chrono>

namespace Net {
    NetClock::NetClock(const uint32_t tickRate) : tickRate(tickRate) {}

    void NetClock::reset(const uint32_t tickValue) {
        tick = tickValue;
    }

    void NetClock::advance(const uint32_t ticks) {
        tick += ticks;
    }

    uint32_t NetClock::getTick() const {
        return tick;
    }

    uint32_t NetClock::getTickRate() const {
        return tickRate;
    }

    uint32_t NetClock::nowMs() const {
        using namespace std::chrono;
        return static_cast<uint32_t>(duration_cast<milliseconds>(
            steady_clock::now().time_since_epoch()).count());
    }
} // Net
