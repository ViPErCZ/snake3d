#ifndef SNAKE3_ANIMATIONARRAYMESH_H
#define SNAKE3_ANIMATIONARRAYMESH_H

#include <memory>

#include <snake3d/Renderer/Opengl/Model/Standard/StandardMesh.h>
#include <snake3d/Renderer/Opengl/Model/Standard/Animation/AnimationPlayer.h>

namespace Model {
    class AnimationArrayMesh final : public StandardMesh {
    public:
        // Which skinned sub-meshes this node draws every frame:
        //  ClipMesh   - only the sub-mesh bound to the active clip's first bone.
        //               Legacy behaviour for models whose body is split across
        //               several clips, one mesh per clip (e.g. pacman ghost.glb,
        //               which builds one node per pupil clip).
        //  WholeModel - every skinned sub-mesh of the player's single armature,
        //               all sharing the uploaded finalBonesMatrices. For multi-part
        //               characters exported as many meshes on one rig (e.g. KayKit
        //               Human.glb: 48 meshes, 62 joints).
        enum class SkinScope { ClipMesh, WholeModel };

        AnimationArrayMesh(const std::shared_ptr<Animation::AnimationPlayer> &model, const std::shared_ptr<Manager::ShaderProgram>& baseShader, const std::string &animationName);
        // WholeModel-capable overload; pass SkinScope::WholeModel for multi-part rigs.
        AnimationArrayMesh(const std::shared_ptr<Animation::AnimationPlayer> &model, const std::shared_ptr<Manager::ShaderProgram>& baseShader, const std::string &animationName, SkinScope scope);
        void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform, bool shadows) const override;
        void renderShadowMap(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt, const glm::mat4 &parentTransform) const override;

    protected:
        void renderMesh(const glm::mat4 &parentTransform, bool animPlay = true) const;
        std::shared_ptr<Manager::ShaderProgram> baseShader;
        // false -> ClipMesh (legacy single-mesh-per-clip); true -> WholeModel.
        bool drawAllSkinned = false;
    };
} // Model

#endif //SNAKE3_ANIMATIONARRAYMESH_H