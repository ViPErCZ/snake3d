#ifndef SNAKE3_OBJWALLRENDERER_H
#define SNAKE3_OBJWALLRENDERER_H

#include "../../Manager/ShaderManager.h"
#include "../../Manager/ResourceManager.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../ItemsDto/ObjWall.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class ObjWallRenderer : public BaseRenderer {
    public:
        explicit ObjWallRenderer(ObjWall *item, Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        ~ObjWallRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override;
        void toggleParallax();
        void downScale();
        void upScale();

    protected:
        ObjWall* wall;
        Mesh* mesh;
        Camera* camera;
        glm::mat4 projection{};
        ResourceManager* resourceManager;
        ShaderManager* shader;
        TextureManager* texture1;
        TextureManager* texture2;
        TextureManager* texture3;
        TextureManager* texture4;
        bool parallax;
        float heightScale;
    };

} // Renderer

#endif //SNAKE3_OBJWALLRENDERER_H
