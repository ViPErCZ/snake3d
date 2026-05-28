#include <snake3d/Renderer/Opengl/Material/Uniform/FadeOutUniform.h>

#include <functional>

using namespace std;
using namespace Tools;
using namespace Manager;
using namespace Material;

namespace Uniform {
    FadeOutUniform::FadeOutUniform()
        : running(false), alpha(1.0f), step(0.5f) {
        timer = make_unique<Timer>(false);
    }

    void FadeOutUniform::bind(const shared_ptr<ShaderProgram> &shader, const string &name) {
        if (timer->isRunning()) {
            if (isFinished() == false) {
                alpha -= step * static_cast<float>(timer->getDeltaTime());
                alpha = glm::clamp(alpha, 0.0f, 1.0f);
                running = true;
            }  else {
                running = false;
                timer->stop();
                if (finished) {
                    finished();
                }
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

    void FadeOutUniform::start() {
        alpha = 1;
        timer->reset();
        timer->start();
    }

    shared_ptr<IUniform> FadeOutUniform::clone() const {
        auto cloned = make_shared<FadeOutUniform>();

        return cloned;
    }

    void FadeOutUniform::setStep(const float step) {
        this->step = step;
    }

    void FadeOutUniform::setFinishedCallback(const std::function<void()> &callback) {
        finished = callback;
    }

    bool FadeOutUniform::isFinished() const {
        return running && alpha <= 0;
    }
} // Uniform
