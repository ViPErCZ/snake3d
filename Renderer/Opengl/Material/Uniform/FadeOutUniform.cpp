#include "FadeOutUniform.h"

namespace Uniform {
    FadeOutUniform::FadeOutUniform()
        : running(false), alpha(1.0f), step(0.5f) {
        timer = make_unique<Timer>(false);
    }

    void FadeOutUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        if (timer->isRunning()) {
            if (isFinished() == false) {
                alpha -= step * static_cast<float>(timer->getDeltaTime());
                running = true;
            }  else {
                running = false;
                timer->stop();
            }
        }
        shader->setUniform(name, alpha);
        timer->update();
    }

    void FadeOutUniform::setAlpha(const float alpha) {
        this->alpha = alpha;
    }

    float FadeOutUniform::getAlpha() const {
        return alpha;
    }

    void FadeOutUniform::start() const {
        timer->start();
    }

    bool FadeOutUniform::isFinished() const {
        return running && alpha <= 0;
    }
} // Uniform
