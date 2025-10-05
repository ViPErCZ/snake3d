#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../Renderer/Opengl/BloomRenderer.h"
#include <vector>
#include <memory>

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
        RenderManager(int width, int height, const shared_ptr<Camera> &camera);
        ~RenderManager();
        void render(float dt);
        void addRenderer(BaseRenderer* renderer);
        void setDepthMapRenderer(DepthMapRenderer *depthMapRenderer);
        void setBloomRenderer(BloomRenderer *bloomRenderer);
        void setWidth(int width);
        void setHeight(int height);
        void enableShadows();
        void disableShadows();
        void toggleShadows();
        void toggleBloom();
        void toggleFog();

    protected:
        void updateShadows();
        void updateFog();
        vector<BaseRenderer*> renderers;
        DepthMapRenderer* depthMapRenderer{};
        BloomRenderer* bloomRenderer{};
        shared_ptr<Camera> camera;
        int width;
        int height;
        bool shadows;
        bool bloom;
        bool fog;
    };

} // Manager

#endif //SNAKE3_RENDERMANAGER_H
