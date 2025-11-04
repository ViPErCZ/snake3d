#ifndef SNAKE3_LABELNODE2D_H
#define SNAKE3_LABELNODE2D_H

#include "BaseNode2D.h"
#include "../../../Material/2D/LabelSettings.h"

using namespace std;
using namespace Material;

namespace Model {
    class LabelNode2D final : public BaseNode2D {
    public:
        explicit LabelNode2D(const std::string& text, const shared_ptr<ShaderManager> &baseShader,
                       const shared_ptr<LabelSettings> &settings);

        ~LabelNode2D() override = default;

        [[nodiscard]] unsigned int getTextureId() const { return textureId; }

    private:
        unsigned int textureId = 0;

    };
} // Model

#endif //SNAKE3_LABELNODE2D_H