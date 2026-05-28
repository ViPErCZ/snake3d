#ifndef SNAKE3_IMAGENODE2D_H
#define SNAKE3_IMAGENODE2D_H

#include <memory>

#include "BaseNode2D.h"
#include <snake3d/Manager/TextureManager.h>

namespace Model {
    using std::shared_ptr;

    class ImageNode2D final : public BaseNode2D {
    public:
        ImageNode2D(float width, float height,
                    const shared_ptr<Manager::ShaderProgram> &baseShader,
                    const shared_ptr<Manager::TextureManager> &texture);

        void render(const shared_ptr<Manager::Camera> &camera, const glm::mat4 &ortho, float dt,
                    const glm::mat4 &parentTransform) const override;

        [[nodiscard]] const glm::vec2 &getSize() const;

    private:
        shared_ptr<Manager::TextureManager> texture;
        glm::vec2 size{0.0f, 0.0f};
    };
} // Model

#endif // SNAKE3_IMAGENODE2D_H
