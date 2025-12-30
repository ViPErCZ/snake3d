#ifndef SNAKE3_PLANARREFLECTIONRENDERER_H
#define SNAKE3_PLANARREFLECTIONRENDERER_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "BaseRenderer.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/Camera.h"
#include "Scene/SceneRenderer.h"

using namespace std;
using namespace Manager;

namespace Renderer {
    class PlanarReflectionRenderer final : public BaseRenderer {
    public:
        PlanarReflectionRenderer(const shared_ptr<ResourceManager> &resManager, 
                                 const shared_ptr<Camera> &camera, 
                                 const glm::mat4 &projection,
                                 int width, int height);
        ~PlanarReflectionRenderer() override;

        void updateRenderers(const vector<RendererEntry> &renderers);
        void render3D(float dt, uint64_t frameId) override;
        void beforeRender(MODE mode) override;
        void afterRender() override;
        void renderShadowMap() override;
        void setPlaneZ(float z);

    protected:
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        vector<Scenes::RendererEntry3D> nodes3d;
        vector<RendererEntry> renderers;

        unsigned int reflectionFBO{};
        unsigned int reflectionTexture{};
        unsigned int depthBuffer{};
        int width;
        int height;
        float planeZ = -1.0f;
    };
}

#endif //SNAKE3_PLANARREFLECTIONRENDERER_H
