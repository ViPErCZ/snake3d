#ifndef SNAKE3_TEXTUREUNIFORM_H
#define SNAKE3_TEXTUREUNIFORM_H

#include "../IUniform.h"
#include "../../../../Manager/TextureManager.h"

using namespace Material;
using namespace Manager;
using namespace std;

namespace Uniform {
    class TextureUniform final : public IUniform {
    public:
        TextureUniform(int index, const shared_ptr<TextureManager> &texture);

        void bind(const shared_ptr<ShaderManager> &shader, const string &name) override;

        [[nodiscard]] shared_ptr<IUniform> clone() const override;

    protected:
        shared_ptr<TextureManager> texture;
        int index;
    };
} // Uniform

#endif //SNAKE3_TEXTUREUNIFORM_H
