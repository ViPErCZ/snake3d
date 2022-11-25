#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
#include "../Renderer/Opengl/DepthMapRenderer.h"
#include "../stdafx.h"
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
        void setWidth(int width);
        void setHeight(int height);

    protected:
        vector<BaseRenderer*> renderers;
        DepthMapRenderer* depthMapRenderer{};
        int width;
        int height;
    };

} // Manager

#endif //SNAKE3_RENDERMANAGER_H
