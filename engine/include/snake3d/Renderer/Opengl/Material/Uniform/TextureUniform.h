#ifndef SNAKE3_TEXTUREUNIFORM_H
#define SNAKE3_TEXTUREUNIFORM_H

#include <snake3d/Renderer/Opengl/Material/IUniform.h>
#include <snake3d/Manager/TextureManager.h>

namespace Uniform {
    class TextureUniform final : public Material::IUniform {
    public:
        TextureUniform(int index, const std::shared_ptr<Manager::TextureManager> &texture, bool use_cube = false);

        void bind(const std::shared_ptr<Manager::ShaderProgram> &shader, const std::string &name) override;

        [[nodiscard]] std::shared_ptr<Material::IUniform> clone() const override;

    protected:
        std::shared_ptr<Manager::TextureManager> texture;
        int index;
        bool cube;
    };
} // Uniform

#endif //SNAKE3_TEXTUREUNIFORM_H
