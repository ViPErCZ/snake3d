#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../stdafx.h"
#include "../Renderer/Opengl/BloomRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace Renderer;

namespace Manager {

    class RenderManager {
    public:
        RenderManager(int width, int height);
        virtual ~RenderManager();
        void render();
        void addRenderer(BaseRenderer* renderer);
        void setDepthMapRenderer(DepthMapRenderer *depthMapRenderer);
        void setBloomRenderer(BloomRenderer *bloomRenderer);
        void setWidth(int width);
        void setHeight(int height);
        void enableShadows();
        void disableShadows();
        void toggleShadows();

    protected:
        void updateShadows();
        vector<BaseRenderer*> renderers;
        DepthMapRenderer* depthMapRenderer{};
        BloomRenderer* bloomRenderer{};
        int width;
        int height;
        bool shadows{};
    };

} // Manager

#endif //SNAKE3_RENDERMANAGER_H
