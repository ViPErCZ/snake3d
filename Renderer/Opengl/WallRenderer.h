#ifndef SNAKE3_WALLRENDERER_H
#define SNAKE3_WALLRENDERER_H

#include "../../ItemsDto/Wall.h"
#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class WallRenderer : public BaseRenderer {
    public:
        explicit WallRenderer(Wall *item);
        ~WallRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Wall* wall;
        ShaderManager* shaderManager;
    };

} // Renderer

#endif //SNAKE3_WALLRENDERER_H
