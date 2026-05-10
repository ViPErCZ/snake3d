#ifndef SNAKE3_TIMER_H
#define SNAKE3_TIMER_H

namespace Tools {
    class Timer {
    public:
        explicit Timer(bool autoStart = false);

        void start();

        void stop();

        void reset();

        [[nodiscard]] bool isRunning();

        double getElapsedTime();

        [[nodiscard]] double getDeltaTime();

        void update();

        [[nodiscard]] double getWaitTime() const;

        [[nodiscard]] double getNow() const;

    private:
        bool autoStart;
        bool running;
        float waitTime;
        double now;
        double lastTime;
        double startTime;
        bool waitTimeLeave;
    };
} // Tools

#endif //SNAKE3_TIMER_H
