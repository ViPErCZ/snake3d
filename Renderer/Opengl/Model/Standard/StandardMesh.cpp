#include "StandardMesh.h"
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>

#include "../../../../Tools/ContextState.h"
#include "../../Material/MaterialInstance.h"
#include "../../Material/RenderContext.h"
#include "../../Material/ShaderMaterial.h"
#include "../../RenderStats.h"

using namespace Animation;
using namespace Manager;
using namespace Material;
using namespace ModelUtils;
using namespace Tools;
using namespace std;

namespace Model {
    StandardMesh::StandardMesh(shared_ptr<ShaderProgram> baseShader)
        : baseShader(std::move(baseShader)), localMin(+FLT_MAX), localMax(-FLT_MIN), worldMin(+FLT_MAX), worldMax(-FLT_MIN) {
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

    void StandardMesh::bind() const {
        mesh->bind();
    }

    unsigned long StandardMesh::indicesCount() const {
        return mesh->getIndices().size();
    }

    void StandardMesh::render(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
                              const glm::mat4 &parentTransform, const bool shadows) const {

        glm::mat4 worldTransform = parentTransform;
        float alpha = 1.0f;

        if (nullptr != animationPlayer && !animation.empty()) {
            const auto metadata = animationPlayer->play(animation);
            worldTransform = worldTransform * metadata->world_transform;
            alpha = metadata->alpha;
        }

        if (const auto materialInstance = std::dynamic_pointer_cast<MaterialInstance>(material)) {
            const RenderContext ctx{
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                worldTransform,
                static_cast<float>(glfwGetTime()),
                shadows
            };
            materialInstance->bind(ctx);
        } else if (const auto shaderMaterial = std::dynamic_pointer_cast<ShaderMaterial>(material)) {
            shaderMaterial->bind(
                camera->getPosition(),
                camera->getViewMatrix(),
                projection,
                worldTransform,
                shadows
            );
        } else {
            // Fallback branch: mesh has no MaterialInstance / ShaderMaterial.
            // D1.1b: view/projection/viewPos arrive via FrameData UBO bound
            // by RenderManager once per pass - no per-draw setMat4 needed.
            (void)alpha;
            baseShader->use();
            baseShader->setMat4("model", worldTransform);
            baseShader->setBool("useBones", false);
            baseShader->setBool("shadowsEnable", false);
            baseShader->setBool("iblEnabled", false);
            // D1.1c: directionLightEnable migrated to MaterialData UBO; the
            // fallback branch doesn't drive a MaterialInstance so the UBO
            // shadow stays at its zero default -- which is also the desired
            // off state. Nothing to set here.
            // D1.1d: numPointLights / numSpotLights migrated to MaterialData
            // UBO as well (material_numPointLights / material_numSpotLights).
            // Same story -- UBO defaults to 0, so no per-draw setInt needed.
        }

        mesh->bind();
        Renderer::RenderStats::countDraw();
        glDrawElements(static_cast<GLenum>(drawElement), static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                       nullptr);
        if (const auto materialInstance = std::dynamic_pointer_cast<const MaterialInstance>(material)) {
            materialInstance->unbind();
        } else if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            shaderMaterial->unbind();
        }
    }

    void StandardMesh::renderShadowMap(const shared_ptr<Camera> &camera, const glm::mat4 &projection, float dt,
        const glm::mat4 &parentTransform) const {
        if (const auto materialInstance = std::dynamic_pointer_cast<const MaterialInstance>(material)) {
            if (materialInstance->bindShadow(parentTransform)) {
                mesh->bind();
                Renderer::RenderStats::countDraw();
                glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                               nullptr);
            }
            return;
        }
        if (const auto shaderMaterial = std::dynamic_pointer_cast<const ShaderMaterial>(material)) {
            if (shaderMaterial->isShadowEnabled() && shaderMaterial->getShadowDepthShader()) {
                shaderMaterial->bindShadow(parentTransform);
                mesh->bind();
                Renderer::RenderStats::countDraw();
                glDrawElements(GL_TRIANGLES, static_cast<int>(mesh->getIndices().size()), GL_UNSIGNED_INT,
                               nullptr);
                shaderMaterial->unbind();
            }
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

    void StandardMesh::setBlending(const Blending blending) {
        this->blending = blending;
    }

    void StandardMesh::setDepthTest(const bool depthTest) {
        this->depthTest = depthTest;
    }

    bool StandardMesh::getDepthTest() const {
        return depthTest;
    }

    void StandardMesh::setDepthWrite(const bool depthWrite) {
        this->depthWrite = depthWrite;
    }

    bool StandardMesh::getDepthWrite() const {
        return depthWrite;
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

    void StandardMesh::computeLocalAABB() {
        const auto vertices = mesh->getVertices();
        localMin = vertices[0].position;
        localMax = vertices[0].position;
        for (const auto &v: vertices) {
            localMin = glm::min(localMin, v.position);
            localMax = glm::max(localMax, v.position);
        }
    }
} // Model
