#include "FadeInUniform.h"

using namespace std;
using namespace Manager;

namespace Uniform {
    void FadeInUniform::bind(const shared_ptr<ShaderProgram> &shader, const string &name) {
        if (timer->isRunning()) {
            if (isFinished() == false) {
                alpha += step * static_cast<float>(timer->getDeltaTime());
                alpha = glm::clamp(alpha, 0.0f, 1.0f);
                running = true;
            } else {
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

    void FadeInUniform::start() {
        alpha = 0;
        timer->reset();
        timer->start();
    }

    bool FadeInUniform::isFinished() const {
        return running && alpha >= 1;
    }
} // Uniform