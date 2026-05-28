#ifndef SNAKE3_STANDARDMESH_H
#define SNAKE3_STANDARDMESH_H

#include <memory>
#include "../Utils/Mesh.h"
#include "../../Material/BaseMaterial.h"
#include "../../../../Manager/ShaderProgram.h"
#include <snake3d/Tools/DrawElement.h>
#include "../../../../Manager/Camera.h"
#include "Animation/AnimationPlayer.h"

namespace Model {
    class StandardMesh {
    public:
        virtual ~StandardMesh() = default;

        explicit StandardMesh(std::shared_ptr<Manager::ShaderProgram> baseShader);

        [[nodiscard]] std::shared_ptr<ModelUtils::Mesh> getMesh() const;

        void setMaterial(const std::shared_ptr<Material::BaseMaterial> &material);

        [[nodiscard]] std::shared_ptr<Material::BaseMaterial> getMaterial() const;

        void bind() const;

        [[nodiscard]] unsigned long indicesCount() const;

        virtual void render(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                            const glm::mat4 &parentTransform, bool shadows) const;

        virtual void update(float dt) {
        };

        virtual void renderShadowMap(const std::shared_ptr<Manager::Camera> &camera, const glm::mat4 &projection, float dt,
                                     const glm::mat4 &parentTransform) const;

        [[nodiscard]] glm::vec3 getMin(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] glm::vec3 getMax(const glm::mat4 &worldMatrix) const;

        [[nodiscard]] std::shared_ptr<StandardMesh> deepCopy() const;

        void setAnimationPlayer(const std::shared_ptr<Animation::AnimationPlayer> &animationPlayer);

        [[nodiscard]] const std::shared_ptr<Animation::AnimationPlayer> &getAnimationPlayer() const;

        void setBlending(Tools::Blending blending);

        void setDepthTest(bool depthTest);

        void setDepthWrite(bool depthWrite);

        [[nodiscard]] Tools::Blending getBlending() const;

        [[nodiscard]] bool getDepthTest() const;

        [[nodiscard]] bool getDepthWrite() const;

        void animationPlay(const std::string &name, bool loop = true);

        void animationStop(const std::string &name) const;

        void animationPause(const std::string &name) const;

        void animationResume(const std::string &name) const;

        void computeLocalAABB();

    protected:
        std::shared_ptr<ModelUtils::Mesh> mesh;
        std::shared_ptr<Material::BaseMaterial> material;
        std::shared_ptr<Manager::ShaderProgram> baseShader;
        std::shared_ptr<Animation::AnimationPlayer> animationPlayer;
        glm::vec3 localMin;
        glm::vec3 localMax;
        glm::vec3 worldMin;
        glm::vec3 worldMax;
        Tools::Blending blending = Tools::Blending::Opaque;
        bool depthTest = true;
        bool depthWrite = true;
        std::string animation;
        Tools::DrawElement drawElement = Tools::DrawElement::Triangles;
    };
} // Model

#endif //SNAKE3_STANDARDMESH_H
