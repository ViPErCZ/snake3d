#ifndef SNAKE3_CALLBACKUNIFORM_H
#define SNAKE3_CALLBACKUNIFORM_H

#include <functional>

#include <snake3d/Renderer/Opengl/Material/IUniform.h>

namespace Uniform {
    class CallbackUniform : public Material::IUniform {
    public:
        using CallbackType = std::function<void(const std::string& name, const std::shared_ptr<Manager::ShaderProgram>& shader)>;

        explicit CallbackUniform(CallbackType callback);

        void bind(const std::shared_ptr<Manager::ShaderProgram> &shader, const std::string &name) override;

        [[nodiscard]] std::shared_ptr<Material::IUniform> clone() const override;

    protected:
        CallbackType callback;
    };
} // Uniform

#endif //SNAKE3_CALLBACKUNIFORM_H
