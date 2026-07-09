#include <snake3d/Renderer/Opengl/Material/Uniform/TextureArrayUniform.h>

using namespace std;
using namespace Manager;
using namespace Material;

namespace Uniform {
    TextureArrayUniform::TextureArrayUniform(const int index, const shared_ptr<TextureManager> &texture) : texture(texture),
        index(index) {
    }

    void TextureArrayUniform::bind(const shared_ptr<ShaderProgram> &shader, const string &name) {
        texture->bindArr(index, 0);
        shader.get()->setUniform(name, index);
    }

    shared_ptr<IUniform> TextureArrayUniform::clone() const {
        auto cloned = make_shared<TextureArrayUniform>(index, texture);

        return cloned;
    }
} // Uniform
