#ifndef SNAKE3_OBJWALLRENDERER_H
#define SNAKE3_OBJWALLRENDERER_H

#include "../../Manager/ShaderManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../ItemsDto/ObjWall.h"
#include "Model/WallModel.h"

using namespace ItemsDto;
using namespace Manager;
using namespace Model;

namespace Renderer {

    class ObjWallRenderer : public BaseRenderer {
    public:
        explicit ObjWallRenderer(ObjWall *item, ShaderManager* shader, Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        ~ObjWallRenderer() override;
        void reCreate();
        void render() override;
        void beforeRender() override;
        void afterRender() override;

    protected:
        ObjWall* wall;
        WallModel* model;
        ShaderManager* shaderManager;
        Camera* camera;
        glm::mat4 projection{};
        ResourceManager* resourceManager;
    };

} // Renderer

#endif //SNAKE3_OBJWALLRENDERER_H
