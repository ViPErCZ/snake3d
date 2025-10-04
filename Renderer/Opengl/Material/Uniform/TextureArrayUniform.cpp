#include "TextureArrayUniform.h"

namespace Uniform {
    TextureArrayUniform::TextureArrayUniform(const int index, const shared_ptr<TextureManager> &texture) : texture(texture),
        index(index) {
    }

    void TextureArrayUniform::bind(const shared_ptr<ShaderManager> &shader, const string &name) {
        texture->bindArr(index, 0);
        shader.get()->setUniform(name, index);
    }
} // Uniform
