#ifndef SNAKE3_TIMERUNIFORM_H
#define SNAKE3_TIMERUNIFORM_H

#include "../IUniform.h"
#include "../../../../Tools/Timer.h"
#include <memory>

using namespace Tools;
using namespace Material;
using namespace Manager;
using namespace std;

namespace Uniform {
    class TimerUniform final : public IUniform {
    public:
        explicit TimerUniform(bool autostart = false);

        void bind(const shared_ptr<ShaderManager> &shader, const string &name) override;

        [[nodiscard]] shared_ptr<IUniform> clone() const override;

        [[nodiscard]] double getElapsed() const;

        [[nodiscard]] bool isRunning() const;

        void stop() const;

        void start() const;

        void reset() const;

    private:
        unique_ptr<Timer> timer;
        bool autostart;
    };
} // Uniform

#endif //SNAKE3_TIMERUNIFORM_H
