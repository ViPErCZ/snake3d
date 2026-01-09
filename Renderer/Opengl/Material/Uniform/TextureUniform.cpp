#include "TextureUniform.h"

namespace Uniform {
    TextureUniform::TextureUniform(const int index, const shared_ptr<TextureManager> &texture, const bool use_cube)
        : texture(texture), index(index), cube(use_cube) {
    }

    void TextureUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        if (cube) {
            texture->cubeBind(index);
        } else {
            texture->bind(index, 0);
        }
        shader->setUniform(name, index);
    }

    shared_ptr<IUniform> TextureUniform::clone() const {
        auto cloned = make_shared<TextureUniform>(index, texture);

        return cloned;
    }
} // Uniform
