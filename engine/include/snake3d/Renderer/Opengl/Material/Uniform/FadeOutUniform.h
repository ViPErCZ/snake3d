#ifndef SNAKE3_FADEOUTUNIFORM_H
#define SNAKE3_FADEOUTUNIFORM_H

#include <functional>

#include <snake3d/Renderer/Opengl/Material/IUniform.h>
#include <snake3d/Tools/Timer.h>

namespace Uniform {
    class FadeOutUniform : public Material::IUniform {
    public:
        explicit FadeOutUniform();
        void bind(const std::shared_ptr<Manager::ShaderProgram>& shader, const std::string& name) override;
        void setAlpha(float alpha);
        [[nodiscard]] float getAlpha() const;
        virtual void start();
        [[nodiscard]] std::shared_ptr<Material::IUniform> clone() const override;
        void setStep(float step);
        void setFinishedCallback(const std::function<void()> &callback);
    protected:
        [[nodiscard]] virtual bool isFinished() const;

        bool running;
        float alpha;
        float step;
        std::unique_ptr<Tools::Timer> timer;
        std::function<void()> finished;
    };
} // Uniform

#endif //SNAKE3_FADEOUTUNIFORM_H