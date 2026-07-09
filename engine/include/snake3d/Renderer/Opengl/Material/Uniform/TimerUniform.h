#ifndef SNAKE3_TIMERUNIFORM_H
#define SNAKE3_TIMERUNIFORM_H

#include <snake3d/Renderer/Opengl/Material/IUniform.h>
#include <snake3d/Tools/Timer.h>
#include <memory>

namespace Uniform {
    class TimerUniform final : public Material::IUniform {
    public:
        explicit TimerUniform(bool autostart = false);

        void bind(const std::shared_ptr<Manager::ShaderProgram> &shader, const std::string &name) override;

        [[nodiscard]] std::shared_ptr<Material::IUniform> clone() const override;

        [[nodiscard]] double getElapsed() const;

        [[nodiscard]] bool isRunning() const;

        void stop() const;

        void start() const;

        void reset() const;

    private:
        std::unique_ptr<Tools::Timer> timer;
        bool autostart;
    };
} // Uniform

#endif //SNAKE3_TIMERUNIFORM_H
