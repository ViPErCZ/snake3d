#ifndef SNAKE3_RAINDROPRENDERER_H
#define SNAKE3_RAINDROPRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/BaseItem.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "Model/MeshModel.h"

using namespace ItemsDto;
using namespace Model;
using namespace Manager;

namespace Renderer {

    class RainDropRenderer : public BaseRenderer {
    public:
        RainDropRenderer(BaseItem *item, Camera *camera, glm::mat4 proj, ResourceManager *resourceManager);
        ~RainDropRenderer() override;
        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void setEnable(bool enable);

    protected:
        ResourceManager *resourceManager;
        ShaderManager *baseShader;
        Camera *camera;
        MeshModel* model;
        TextureManager* texture;
        glm::mat4 projection{};
        float lastFrame = 0.0f;
        bool enable;
    };

} // Renderer

#endif //SNAKE3_RAINDROPRENDERER_H
