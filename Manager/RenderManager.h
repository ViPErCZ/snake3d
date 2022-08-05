#ifndef SNAKE3_RENDERMANAGER_H
#define SNAKE3_RENDERMANAGER_H

#include "../Renderer/Opengl/BaseRenderer.h"
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
        void setStickyPoint(BaseItem *stickyPoint);
        void setWidth(int width);
        void setHeight(int height);

    protected:
        vector<BaseRenderer*> renderers;
        void setupProjection();
        void switchOrthoProjection() const;
        static void restoreProjection();
        BaseItem* stickyPoint{};
        int width;
        int height;
        float test = 0;
    };

} // Manager

#endif //SNAKE3_RENDERMANAGER_H
