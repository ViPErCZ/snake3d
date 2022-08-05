#ifndef SNAKE3_OBJWALLRENDERER_H
#define SNAKE3_OBJWALLRENDERER_H

#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../ItemsDto/ObjWall.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class ObjWallRenderer : public BaseRenderer {
    public:
        explicit ObjWallRenderer(ObjWall *item);
        ~ObjWallRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;

    protected:
        ObjWall* wall;
        ShaderManager* shaderManager;
    };

} // Renderer

#endif //SNAKE3_OBJWALLRENDERER_H
