#include <snake3d/Renderer/Opengl/Material/Uniform/TimerUniform.h>

using namespace std;
using namespace Tools;
using namespace Manager;
using namespace Material;

namespace Uniform {
    TimerUniform::TimerUniform(bool autostart) : autostart(autostart) {
        timer = make_unique<Timer>(autostart);
    }

    void TimerUniform::bind(const shared_ptr<ShaderProgram> &shader, const string &name) {
        timer->update();
        shader->setUniform(name, static_cast<float>(timer->getElapsedTime()));
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

    void TimerUniform::reset() const {
        timer->reset();
    }

    bool TimerUniform::isRunning() const {
        return timer->isRunning();
    }

    double TimerUniform::getElapsed() const {
        return timer->getElapsedTime();
    }
} // Uniform