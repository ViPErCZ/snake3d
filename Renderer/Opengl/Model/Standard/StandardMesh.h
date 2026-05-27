#ifndef SNAKE3_STANDARDMESH_H
#define SNAKE3_STANDARDMESH_H

#include <memory>
#include "../Utils/Mesh.h"
#include "../../Material/BaseMaterial.h"
#include "../../../../Manager/ShaderProgram.h"
#include "../../../../Tools/DrawElement.h"
#include "../../../../Manager/Camera.h"
#include "Animation/AnimationPlayer.h"

using namespace ModelUtils;
using namespace Material;
using namespace Tools;
using namespace std;

namespace Model {
    class StandardMesh {
    public:
        virtual ~StandardMesh() = default;

        explicit StandardMesh(shared_ptr<ShaderProgram> baseShader);

        [[nodiscard]] shared_ptr<Mesh> getMesh() const;

        void setMaterial(const shared_ptr<BaseMaterial> &material);

        [[nodiscard]] shared_ptr<BaseMaterial> getMaterial() const;

        void bind() const;

        [[nodiscard]] unsigned long indicesCount() const;

        virtual void render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                            const glm::mat4 &parentTransform, bool shadows) const;

        virtual void update(float dt) {
        };

        virtual void renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                                     const glm::mat4 &parentTransform) const;

        [[nodiscard]] glm::vec3 getMin(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] glm::vec3 getMax(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] shared_ptr<StandardMesh> deepCopy() const;

        void setAnimationPlayer(const shared_ptr<Animation::AnimationPlayer> &animationPlayer);

        [[nodiscard]] const shared_ptr<Animation::AnimationPlayer> &getAnimationPlayer() const;

        void setBlending(Blending blending);

        void setDepthTest(bool depthTest);

        void setDepthWrite(bool depthWrite);

        [[nodiscard]] Blending getBlending() const;

        [[nodiscard]] bool getDepthTest() const;

        [[nodiscard]] bool getDepthWrite() const;

        void animationPlay(const string &name, bool loop = true);

        void animationStop(const string &name) const;

        void animationPause(const string &name) const;

        void animationResume(const string &name) const;

        void computeLocalAABB();

    protected:
        shared_ptr<Mesh> mesh;
        shared_ptr<BaseMaterial> material;
        shared_ptr<ShaderProgram> baseShader;
        shared_ptr<Animation::AnimationPlayer> animationPlayer;
        glm::vec3 localMin;
        glm::vec3 localMax;
        glm::vec3 worldMin;
        glm::vec3 worldMax;
        Blending blending = Blending::Opaque;
        bool depthTest = true;
        bool depthWrite = true;
        string animation;
        DrawElement drawElement = DrawElement::Triangles;
    };
} // Model

#endif //SNAKE3_STANDARDMESH_H
