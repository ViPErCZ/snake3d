#ifndef SNAKE3_FADEINUNIFORM_H
#define SNAKE3_FADEINUNIFORM_H

#include <snake3d/Renderer/Opengl/Material/Uniform/FadeOutUniform.h>

namespace Uniform {
    class FadeInUniform final : public FadeOutUniform {
    public:
        explicit FadeInUniform() {
            alpha = 0.0f;
        }

        void bind(const std::shared_ptr<Manager::ShaderProgram> &shader, const std::string &name) override;

        void start() override;

    private:
        [[nodiscard]] bool isFinished() const override;
    };
} // Uniform

#endif //SNAKE3_FADEINUNIFORM_H
