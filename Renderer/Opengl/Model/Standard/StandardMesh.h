#ifndef SNAKE3_STANDARDMESH_H
#define SNAKE3_STANDARDMESH_H

#include <memory>
#include "../Utils/Mesh.h"
#include "../../Material/StandardMaterial.h"
#include "../../../../Manager/ShaderManager.h"
#include "../../../../Manager/Camera.h"
#include "Animation/AnimationPlayer.h"

using namespace ModelUtils;
using namespace Material;
using namespace Animations;
using namespace std;

namespace Model {
    class StandardMesh {
    public:
        virtual ~StandardMesh() = default;

        explicit StandardMesh(shared_ptr<ShaderManager> baseShader);

        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

        void setMaterial(const shared_ptr<BaseMaterial> &material);

        [[nodiscard]] shared_ptr<BaseMaterial> getMaterial() const;

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                            const glm::mat4 &parentTransform, bool shadows) const;

        virtual void update(float dt) {
        };

        virtual void renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                                     const glm::mat4 &parentTransform) const;

        [[nodiscard]] glm::vec3 getMin(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] glm::vec3 getMax(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] shared_ptr<StandardMesh> deepCopy() const;

        void setAnimationPlayer(const shared_ptr<AnimationPlayer> &animationPlayer);

        [[nodiscard]] const shared_ptr<AnimationPlayer> &getAnimationPlayer() const;

        [[nodiscard]] Blending getBlending() const;

    protected:
        shared_ptr<Mesh> mesh;
        shared_ptr<BaseMaterial> material;
        shared_ptr<ShaderManager> baseShader;
        shared_ptr<AnimationPlayer> animationPlayer;
        glm::vec3 localMin;
        glm::vec3 localMax;
        Blending blending = Blending::Opaque;
    };
} // Model

#endif //SNAKE3_STANDARDMESH_H
