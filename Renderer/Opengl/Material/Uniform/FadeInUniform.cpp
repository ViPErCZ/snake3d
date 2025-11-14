#include "FadeInUniform.h"

namespace Uniform {
    void FadeInUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        if (timer->isRunning()) {
            if (isFinished() == false) {
                alpha += step * static_cast<float>(timer->getDeltaTime());
                running = true;
            } else {
                running = false;
                timer->stop();
            }
        }
        shader->setUniform(name, alpha);
        timer->update();
    }

    bool FadeInUniform::isFinished() const {
        return running && alpha >= 1;
    }
} // Uniform