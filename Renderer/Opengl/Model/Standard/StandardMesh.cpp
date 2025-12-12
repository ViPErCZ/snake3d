#include "StandardMesh.h"
#include <glm/gtx/string_cast.hpp>

#include "../../../../Tools/ContextState.h"

namespace Model {
    StandardMesh::StandardMesh(shared_ptr<ShaderManager> baseShader)
        : baseShader(std::move(baseShader)), localMin(+FLT_MAX), localMax(-FLT_MIN) {
    }

    shared_ptr<Mesh> StandardMesh::getMesh() const {
        return mesh;
    }

    void StandardMesh::setMaterial(const shared_ptr<BaseMaterial> &material) {
        this->material = material;
    }

    shared_ptr<BaseMaterial> StandardMesh::getMaterial() const {
        return material;
    }

    void StandardMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                              const glm::mat4 &parentTransform, const bool shadows) const {

        glm::mat4 worldTransform = parentTransform;
        float alpha = 1.0f;

        if (nullptr != animationPlayer) {
            const auto metadata = animationPlayer->play(animation);
            worldTransform = worldTransform * metadata->world_transform;
            alpha = metadata->alpha;
        }

        if (const auto standardMaterial = std::dynamic_pointer_cast<StandardMaterial>(material)) {
            if (nullptr != animationPlayer) {
                standardMaterial->setAlpha(alpha);
            }
            standardMaterial->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                worldTransform,
                shadows
            );
        } else {
            baseShader->use();
            baseShader->setMat4("view", camera->getViewMatrix());
            baseShader->setMat4("projection", projection);
            baseShader->setMat4("model", worldTransform);
            baseShader->setVec3("viewPos", camera->getPosition());
            baseShader->setBool("useMaterial", true);
            baseShader->setBool("useBones", false);
            baseShader->setBool("shadowsEnable", false);
            baseShader->setBool("iblEnabled", false);
            baseShader->setBool("pbrEnabled", false);
            baseShader->setBool("overrideColorMesh", false);
            baseShader->setFloat("ambientLightColorIntensity", 0.05);
            baseShader->setBool("fogEnable", false);
            baseShader->setInt("numPointLights", 0);
            baseShader->setInt("numSpotLights", 0);
            baseShader->setBool("directionLightEnable", false);
            baseShader->setFloat("alpha", alpha);
        }

        mesh->bind();
        glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                       nullptr);
        if (const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material)) {
            standardMaterial.get()->unbind();
        }
    }

    void StandardMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform) const {
        const auto standardMaterial = std::dynamic_pointer_cast<const StandardMaterial>(material);
        if (standardMaterial) {
            standardMaterial.get()->bindShadow(parentTransform);

            mesh->bind();
            glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                           nullptr);

            standardMaterial.get()->unbind();
        }
    }

    glm::vec3 StandardMesh::getMin(const glm::mat4 &worldMatrix) const {
        const glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMin(+FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = worldMatrix * glm::vec4(corner, 1.0f);
            worldMin = glm::min(worldMin, glm::vec3(worldPos));
        }
        return worldMin;
    }

    glm::vec3 StandardMesh::getMax(const glm::mat4 &worldMatrix) const {
        glm::vec3 corners[8] = {
            {localMin.x, localMin.y, localMin.z},
            {localMin.x, localMin.y, localMax.z},
            {localMin.x, localMax.y, localMin.z},
            {localMin.x, localMax.y, localMax.z},
            {localMax.x, localMin.y, localMin.z},
            {localMax.x, localMin.y, localMax.z},
            {localMax.x, localMax.y, localMin.z},
            {localMax.x, localMax.y, localMax.z}
        };

        glm::vec3 worldMax(-FLT_MAX);
        for (auto corner: corners) {
            glm::vec4 worldPos = worldMatrix * glm::vec4(corner, 1.0f);
            worldMax = glm::max(worldMax, glm::vec3(worldPos));
        }
        return worldMax;
    }

    shared_ptr<StandardMesh> StandardMesh::deepCopy() const {
        auto newMesh = std::make_shared<StandardMesh>(baseShader);

        if (material) {
            // newMesh->setMaterial(material->clone()); // TODO: not implemented
        }

        return newMesh;
    }

    void StandardMesh::setAnimationPlayer(const shared_ptr<AnimationPlayer> &animationPlayer) {
        this->animationPlayer = animationPlayer;
    }

    const shared_ptr<AnimationPlayer> & StandardMesh::getAnimationPlayer() const {
        return animationPlayer;
    }

    Blending StandardMesh::getBlending() const {
        if (material) {
            return material->getBlending();
        }

        return blending;
    }

    void StandardMesh::animationPlay(const string &name, bool loop) {
        if (animationPlayer) {
            animationPlayer->start(name, loop);
            animation = name;
        }
    }

    void StandardMesh::animationStop(const string &name) const {
        if (animationPlayer) {
            animationPlayer->stop(name);
        }
    }

    void StandardMesh::animationPause(const string &name) const {
        if (animationPlayer) {
            animationPlayer->pause(name);
        }
    }

    void StandardMesh::animationResume(const string &name) const {
        if (animationPlayer) {
            animationPlayer->resume(name);
        }
    }
} // Model
