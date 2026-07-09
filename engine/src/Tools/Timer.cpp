#include <snake3d/Tools/Timer.h>

#include <GLFW/glfw3.h>

namespace Tools {
    Timer::Timer(const bool autoStart) : autoStart(autoStart), running(false), waitTime(0.0f), now(0),
                                         lastTime(0), startTime(0), waitTimeLeave(false) {
        if (autoStart) {
            start();
        }
    }

    void Timer::start() {
        running = true;
        now = lastTime = startTime = glfwGetTime();
    }

    void Timer::stop() {
        running = false;
        waitTimeLeave = false;
    }

    void Timer::reset() {
        now = lastTime = startTime = glfwGetTime();
    }

    bool Timer::isRunning() {
        const bool isRunning = running && waitTimeLeave;
        if (false == isRunning) {
            getElapsedTime();
        }

        return isRunning;
    }

    double Timer::getElapsedTime() {
        const double elapsedTime = now - startTime;
        if (!waitTimeLeave) {
            if (elapsedTime < waitTime) {
                return 0.0;
            }
            waitTimeLeave = true;
            lastTime = now;
            return 0.0;
        }

        return elapsedTime;
    }

    void Timer::update() {
        if (!running) {
            return;
        }

        now = glfwGetTime();
    }

    double Timer::getDeltaTime() {
        if (!running) return 0.0;

        const double delta = now - lastTime;
        lastTime = now;

        return delta;
    }

    double Timer::getWaitTime() const {
        return waitTime;
    }

    double Timer::getNow() const {
        return now;
    }
} // Tools
