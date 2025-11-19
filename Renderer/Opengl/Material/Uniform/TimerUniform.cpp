#include "TimerUniform.h"

namespace Uniform {
    TimerUniform::TimerUniform(bool autostart) : autostart(autostart) {
        timer = make_unique<Timer>(autostart);
    }

    void TimerUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        if (timer->isRunning()) {
            shader->setUniform(name, static_cast<float>(timer->getElapsedTime()));
        }
        timer->update();
    }

    shared_ptr<IUniform> TimerUniform::clone() const {
        auto cloned = make_shared<TimerUniform>(autostart);

        return cloned;
    }

    void TimerUniform::stop() const {
        timer->stop();
        timer->reset();
    }

    void TimerUniform::start() const {
        timer->start();
    }

    double TimerUniform::getElapsed() const {
        return timer->getElapsedTime();
    }
} // Uniform