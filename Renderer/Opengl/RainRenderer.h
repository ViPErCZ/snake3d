#ifndef SNAKE3_RAINRENDERER_H
#define SNAKE3_RAINRENDERER_H

#include "../../ItemsDto/BaseItem.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "Model/RainModel.h"
#include "BaseRenderer.h"

using namespace ItemsDto;
using namespace Model;
using namespace Manager;

namespace Renderer {
    class RainRenderer : public BaseRenderer {
    public:
        RainRenderer(BaseItem *item, Camera *camera, glm::mat4 proj, ResourceManager *resourceManager);
        ~RainRenderer() override;
        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void toggle();
        [[nodiscard]] bool isEnable() const;

    protected:
        ResourceManager *resourceManager;
        ShaderManager *baseShader;
        Camera *camera;
        RainModel* model;
        TextureManager* texture;
        glm::mat4 projection{};
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        bool enable;
    };
}

#endif //SNAKE3_RAINRENDERER_H
