#include <snake3d/Renderer/Opengl/Model/Standard/SelectionRingNode3D.h>

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Manager/ShaderProgram.h>
#include <snake3d/Renderer/Opengl/Material/ShaderMaterial.h>
#include <snake3d/Renderer/Opengl/Material/Uniform/CallbackUniform.h>
#include <snake3d/Renderer/Opengl/Model/Standard/ArrayMesh.h>
#include <snake3d/Renderer/Opengl/Model/Standard/PlaneMesh.h>
#include <snake3d/Renderer/Opengl/Model/Utils/Mesh.h>
#include <snake3d/Resource/ShaderLoader.h>
#include <snake3d/Tools/Blending.h>

using namespace std;
using namespace Manager;
using namespace Material;
using namespace Uniform;
using namespace Tools;

namespace Model {
    namespace {
        // Reserved name so all rings in a scene share one compiled program.
        constexpr auto kRingShaderName = "snake3dSelectionRing";
        constexpr auto kRingVs = "EngineShaders/snake3d/selection_ring.vs";
        constexpr auto kRingFs = "EngineShaders/snake3d/selection_ring.fs";
    }

    shared_ptr<ShaderProgram> SelectionRingNode3D::ringShader(const shared_ptr<ResourceManager> &resourceManager) {
        try {
            return resourceManager->getShader(kRingShaderName);
        } catch (...) {
            auto program = make_shared<ShaderProgram>(Resource::ShaderLoader::loadShader(kRingVs, kRingFs));
            resourceManager->addShader(kRingShaderName, program);
            return program;
        }
    }

    SelectionRingNode3D::SelectionRingNode3D(
        const shared_ptr<ContextState> &contextState, const shared_ptr<ResourceManager> &resourceManager,
        const float radius, const glm::vec3 &color)
        : MeshNode3D(contextState, nullptr, resourceManager), radius(radius) {
        const auto program = ringShader(resourceManager);

        material = make_shared<ShaderMaterial>(program);
        material->setBlending(Blending::Translucent);
        material->setUniform("color", color);
        // Slow self-animating pulse (wall-clock driven, no per-frame code needed).
        CallbackUniform::CallbackType pulse = [](const string &name, const shared_ptr<ShaderProgram> &sh) {
            const auto t = static_cast<float>(glfwGetTime());
            sh->setFloat(name, 0.5f + 0.5f * std::sin(t * 3.0f));
        };
        material->setUniform("pulse", make_shared<CallbackUniform>(pulse));

        const auto ringMesh = make_shared<PlaneMesh>(program, 2.0f, 2.0f); // local XZ in [-1,1]
        ringMesh->setMaterial(material);
        ringMesh->setBlending(Blending::Translucent);
        ringMesh->setDepthWrite(false);
        flatMesh = ringMesh;
        mesh = ringMesh;

        setScale(glm::vec3(radius)); // world ring radius
        setVisible(false);           // shown by the game from its selection state
    }

    void SelectionRingNode3D::setColor(const glm::vec3 &color) const {
        if (material) material->setUniform("color", color);
    }

    void SelectionRingNode3D::setGroundPosition(const glm::vec3 &worldPos, const float lift) {
        setPosition((worldPos + glm::vec3(0.0f, lift, 0.0f)) / radius);
        if (heightQuery && glm::distance(worldPos, conformBuiltFor) > 0.01f)
            buildConformingMesh(worldPos);
    }

    void SelectionRingNode3D::setHeightQuery(std::function<float(float, float)> query) {
        heightQuery = std::move(query);
        if (!heightQuery) {
            mesh = flatMesh; // revert to the flat decal
            conformingMesh.reset();
            conformBuiltFor = glm::vec3(1e9f);
            return;
        }
        if (!conformingMesh) {
            conformingMesh = make_shared<ArrayMesh>(material->getShader());
            conformingMesh->setMaterial(material);
            conformingMesh->setBlending(Blending::Translucent);
            conformingMesh->setDepthWrite(false); // same decal contract as the flat mesh
        }
        mesh = conformingMesh;
        conformBuiltFor = glm::vec3(1e9f); // force a rebuild on the next placement
    }

    void SelectionRingNode3D::buildConformingMesh(const glm::vec3 &worldPos) {
        constexpr int kSegments = 40;
        constexpr float kInnerR = 0.58f, kOuterR = 1.0f;
        std::vector<ModelUtils::Vertex> verts;
        verts.reserve((kSegments + 1) * 2);
        for (int s = 0; s <= kSegments; ++s) {
            const float a = glm::two_pi<float>() * static_cast<float>(s) / kSegments;
            for (const float r : {kInnerR, kOuterR}) {
                const float lx = std::sin(a) * r, lz = std::cos(a) * r;
                ModelUtils::Vertex v{};
                v.position = {
                    lx,
                    (heightQuery(worldPos.x + lx * radius, worldPos.z + lz * radius) - worldPos.y) / radius,
                    lz
                };
                v.normal = {0.0f, 1.0f, 0.0f};
                v.color = {1.0f, 1.0f, 1.0f};
                v.texUV = {lx * 0.5f + 0.5f, lz * 0.5f + 0.5f};
                verts.push_back(v);
            }
        }
        std::vector<GLuint> idx;
        idx.reserve(static_cast<size_t>(kSegments) * 6);
        for (GLuint s = 0; s < kSegments; ++s) {
            const GLuint i0 = s * 2, i1 = i0 + 1, i2 = i0 + 2, i3 = i0 + 3;
            idx.insert(idx.end(), {i0, i2, i1, i1, i2, i3});
        }
        conformingMesh->fromMesh(make_shared<ModelUtils::Mesh>(verts, idx, false, "SelectionRingConform"));
        conformBuiltFor = worldPos;
    }
} // Model
