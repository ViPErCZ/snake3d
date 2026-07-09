#include <snake3d/Renderer/Opengl/Node3DRenderer.h>

#include <snake3d/Renderer/Opengl/RenderStats.h>
#include <snake3d/Tools/Frustum.h>

using namespace Model;
using namespace Manager;
using namespace std;

namespace Renderer {
    Node3DRenderer::Node3DRenderer(shared_ptr<Camera> camera,
                                      const glm::mat4 &projection,
                                      shared_ptr<MeshNode3D> rootNode)
        : camera(std::move(camera)),
          rootNode(std::move(rootNode)),
          projection(projection) {
    }

    Node3DRenderer::Node3DRenderer(shared_ptr<Camera> camera, const glm::mat4 &projection)
    : camera(std::move(camera)),
      projection(projection) {
    }

    Node3DRenderer::~Node3DRenderer() = default;

    void Node3DRenderer::render3D(const float dt, const uint64_t frameId) {
        rootNode->update(dt, frameId);
        renderScene();
    }

    void Node3DRenderer::beforeRender(const MODE mode) {
        this->mode = mode;
    }

    void Node3DRenderer::afterRender() {
    }

    void Node3DRenderer::resize(const int width, const int height, const glm::mat4 &projection) {
        this->projection = projection;
    }

    void Node3DRenderer::renderShadowMap() {
        if (shadows) {
            rootNode->renderShadows(camera, projection, 1, glm::mat4(1));
        }
    }

    void Node3DRenderer::setRootNode(const shared_ptr<MeshNode3D> &rootNode) {
        this->rootNode = rootNode;
    }

    shared_ptr<MeshNode3D> Node3DRenderer::getRootNode() {
        return rootNode;
    }

    void Node3DRenderer::renderScene() const {
        if (mode == reflection && !rootNode->isIncludeInPlanarReflection()) {
            return;
        }
        if (mode == refraction && !rootNode->isIncludeInRefraction()) {
            return; // skip only the water surface; the terrain/bed IS refracted
        }

        // Frustum-cull this pass against the camera the pass actually renders with. The
        // PlanarReflectionRenderer physically mirrors the camera for the reflection pass
        // (and restores it after), so getViewMatrix() is already the mirror view here -
        // one expression covers the main, reflection and refraction passes correctly.
        const Tools::Frustum frustum(projection * camera->getViewMatrix());
        Renderer::CullState::frustum = &frustum;
        rootNode->render(camera, projection, 1, glm::mat4(1), shadows);
        Renderer::CullState::frustum = nullptr;
    }
} // Renderer
