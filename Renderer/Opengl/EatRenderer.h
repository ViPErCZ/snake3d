#ifndef SNAKE3_EATRENDERER_H
#define SNAKE3_EATRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Eat.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include <glm/glm.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class EatRenderer : public BaseRenderer {
    public:
        EatRenderer(Eat *eat, Camera *camera, const glm::mat4 &projection,
                    ResourceManager *resourceManager);

        void render(float dt) override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        void renderScene(ShaderManager* shader);
        Eat* eat;
        Mesh* mesh;
        Camera* camera;
        glm::mat4 projection;
        ResourceManager* resourceManager;
        ShaderManager* baseShader;
        ShaderManager* shadowShader;
        TextureManager* texture1;
        TextureManager* texture2;
        TextureManager* texture3;
        TextureManager* depthTexture;
        double lastTime{};
    };

} // Renderer

#endif //SNAKE3_EATRENDERER_H
