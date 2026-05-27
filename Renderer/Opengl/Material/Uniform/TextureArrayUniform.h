#ifndef SNAKE3_TEXTUREARRAYUNIFORM_H
#define SNAKE3_TEXTUREARRAYUNIFORM_H

#include "../IUniform.h"
#include "../../../../Manager/TextureManager.h"

namespace Uniform {
    class TextureArrayUniform final : public Material::IUniform {
    public:
        TextureArrayUniform(int index, const std::shared_ptr<Manager::TextureManager> &texture);
        void bind(const std::shared_ptr<Manager::ShaderProgram>& shader, const std::string& name) override;

        [[nodiscard]] std::shared_ptr<Material::IUniform> clone() const override;

    protected:
        std::shared_ptr<Manager::TextureManager> texture;
        int index;
    };
} // Uniform

#endif //SNAKE3_TEXTUREARRAYUNIFORM_H