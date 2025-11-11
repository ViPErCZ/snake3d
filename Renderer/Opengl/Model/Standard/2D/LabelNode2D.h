#ifndef SNAKE3_LABELNODE2D_H
#define SNAKE3_LABELNODE2D_H

#include "BaseNode2D.h"
#include "../../../Material/2D/LabelSettings.h"

using namespace std;
using namespace Material;

namespace Model {
    class LabelNode2D final : public BaseNode2D {
    public:
        explicit LabelNode2D(std::string text, const shared_ptr<ShaderManager> &baseShader,
                             const shared_ptr<LabelSettings> &settings);

        ~LabelNode2D() override = default;

        [[nodiscard]] unsigned int getTextureId() const { return textureId; }

        void set_texture_id(unsigned int texture_id) {
            textureId = texture_id;
        }

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
            const glm::mat4 &parentTransform) const override;

    protected:
        const shared_ptr<LabelSettings> settings;
        string text;
    };
} // Model

#endif //SNAKE3_LABELNODE2D_H
