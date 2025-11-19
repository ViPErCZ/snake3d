#ifndef SNAKE3_FADEINUNIFORM_H
#define SNAKE3_FADEINUNIFORM_H

#include "FadeOutUniform.h"

namespace Uniform {
    class FadeInUniform final : public FadeOutUniform {
    public:
        explicit FadeInUniform() {
            alpha = 0.0f;
        }

        void bind(const shared_ptr<ShaderManager> &shader, const string &name) override;

        void start() override;

    private:
        [[nodiscard]] bool isFinished() const override;
    };
} // Uniform

#endif //SNAKE3_FADEINUNIFORM_H
