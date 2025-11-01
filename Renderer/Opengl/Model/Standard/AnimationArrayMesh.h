#ifndef SNAKE3_ANIMATIONARRAYMESH_H
#define SNAKE3_ANIMATIONARRAYMESH_H

#include <memory>

#include "StandardMesh.h"
#include "../AnimationModel.h"
#include "Animation/AnimationPlayer.h"

using namespace Animations;

namespace Model {
    class AnimationArrayMesh final : public StandardMesh {
    public:
        AnimationArrayMesh(const std::shared_ptr<AnimationModel> &model, const shared_ptr<ShaderManager>& baseShader);
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform) const override;
        void renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform) const override;
        void stop(bool stop) const;

    protected:
        void renderMesh(const glm::mat4 &parentTransform) const;
        std::shared_ptr<AnimationModel> model;
        shared_ptr<ShaderManager> baseShader;
        shared_ptr<AnimationPlayer> animationPlayer;
    };
} // Model

#endif //SNAKE3_ANIMATIONARRAYMESH_H