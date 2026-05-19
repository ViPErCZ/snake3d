#ifndef SNAKE3_IMAGENODE2D_H
#define SNAKE3_IMAGENODE2D_H

#include <memory>

#include "BaseNode2D.h"
#include "../../../../../Manager/TextureManager.h"

using namespace std;
using namespace Manager;

namespace Model {
    class ImageNode2D final : public BaseNode2D {
    public:
        ImageNode2D(float width, float height,
                    const shared_ptr<ShaderProgram> &baseShader,
                    const shared_ptr<TextureManager> &texture);

        void render(const shared_ptr<Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const override;

        [[nodiscard]] const glm::vec2 &getSize() const;

    private:
        shared_ptr<TextureManager> texture;
        glm::vec2 size{0.0f, 0.0f};
    };
} // Model

#endif // SNAKE3_IMAGENODE2D_H
