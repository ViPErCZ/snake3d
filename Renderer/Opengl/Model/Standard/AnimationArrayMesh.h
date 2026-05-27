#ifndef SNAKE3_ANIMATIONARRAYMESH_H
#define SNAKE3_ANIMATIONARRAYMESH_H

#include <memory>

#include "StandardMesh.h"
#include "Animation/AnimationPlayer.h"

using namespace std;

namespace Model {
    class AnimationArrayMesh final : public StandardMesh {
    public:
        AnimationArrayMesh(const shared_ptr<Animation::AnimationPlayer> &model, const shared_ptr<ShaderProgram>& baseShader, const string &animationName);
        void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform, bool shadows) const override;
        void renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform) const override;

    protected:
        void renderMesh(const glm::mat4 &parentTransform, bool animPlay = true) const;
        shared_ptr<ShaderProgram> baseShader;
    };
} // Model

#endif //SNAKE3_ANIMATIONARRAYMESH_H