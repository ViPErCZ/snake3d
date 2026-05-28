#ifndef SNAKE3_ANIMATIONARRAYMESH_H
#define SNAKE3_ANIMATIONARRAYMESH_H

#include <memory>

#include "StandardMesh.h"
#include "Animation/AnimationPlayer.h"

namespace Model {
    class AnimationArrayMesh final : public StandardMesh {
    public:
        AnimationArrayMesh(const std::shared_ptr<Animation::AnimationPlayer> &model, const std::shared_ptr<Manager::ShaderProgram>& baseShader, const std::string &animationName);
        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform, bool shadows) const override;
        void renderShadowMap(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform) const override;

    protected:
        void renderMesh(const glm::mat4 &parentTransform, bool animPlay = true) const;
        std::shared_ptr<Manager::ShaderProgram> baseShader;
    };
} // Model

#endif //SNAKE3_ANIMATIONARRAYMESH_H