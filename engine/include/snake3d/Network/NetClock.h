#ifndef SNAKE3_NETCLOCK_H
#define SNAKE3_NETCLOCK_H

#include <cstdint>

namespace Net {
    class NetClock final {
    public:
        explicit NetClock(uint32_t tickRate = 60);

        void reset(uint32_t tickValue = 0);
        void advance(uint32_t ticks = 1);

        [[nodiscard]] uint32_t getTick() const;
        [[nodiscard]] uint32_t getTickRate() const;
        [[nodiscard]] uint32_t nowMs() const;

    private:
        uint32_t tickRate = 60;
        uint32_t tick = 0;
    };
} // Net

#endif // SNAKE3_NETCLOCK_H
