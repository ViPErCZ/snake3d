#include "TextureUniform.h"

namespace Uniform {
    TextureUniform::TextureUniform(const int index, const shared_ptr<TextureManager> &texture) : texture(texture),
        index(index) {
    }

    void TextureUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        texture->bind(index, 0);
        shader.get()->setUniform(name, index);
    }
} // Uniform
