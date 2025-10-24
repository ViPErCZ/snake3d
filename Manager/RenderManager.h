#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../Renderer/Opengl/BloomRenderer.h"
#include <vector>

using namespace std;
using namespace Renderer;

namespace Manager {
    enum class RenderPassType {
        Shadow,
        Scene,
        PostProcess
    };

    class RenderManager final {
    public:
        RenderManager(const shared_ptr<Camera> &camera, const shared_ptr<ResourceManager> &resourceManager,
                      const glm::mat4 &projection,
                      int width, int height);

        ~RenderManager() = default;

        void init();

        void render(float dt);

        void addRenderer(shared_ptr<BaseRenderer> renderer);

        void setDepthMapRenderer(unique_ptr<DepthMapRenderer> &depthMapRenderer);

        void setBloomRenderer(unique_ptr<BloomRenderer> &bloomRenderer);

        void setWidth(int width);

        void setHeight(int height);

        void enableShadows();

        void disableShadows();

        void toggleShadows();

        void toggleBloom();

        void toggleFog();

        void reset();

    protected:
        void updateShadows();

        void updateFog();

        vector<shared_ptr<BaseRenderer> > renderers;
        unique_ptr<DepthMapRenderer> depthMapRenderer;
        unique_ptr<BloomRenderer> bloomRenderer;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<Camera> camera;
        glm::mat4 projection{};
        int width;
        int height;
        bool shadows;
        bool bloom;
        bool fog;
    };
} // Manager

#endif //SNAKE3_RENDERMANAGER_H
