#ifndef SNAKE3_FADEOUTUNIFORM_H
#define SNAKE3_FADEOUTUNIFORM_H

#include <functional>

#include "../IUniform.h"
#include "../../../../Tools/Timer.h"

using namespace Tools;
using namespace Material;
using namespace Manager;
using namespace std;

namespace Uniform {
    class FadeOutUniform : public IUniform {
    public:
        explicit FadeOutUniform();
        void bind(const shared_ptr<ShaderProgram>& shader, const string& name) override;
        void setAlpha(float alpha);
        [[nodiscard]] float getAlpha() const;
        virtual void start();
        [[nodiscard]] shared_ptr<IUniform> clone() const override;
        void setStep(float step);
        void setFinishedCallback(const std::function<void()> &callback);
    protected:
        [[nodiscard]] virtual bool isFinished() const;

        bool running;
        float alpha;
        float step;
        unique_ptr<Timer> timer;
        std::function<void()> finished;
    };
} // Uniform

#endif //SNAKE3_FADEOUTUNIFORM_H