#ifndef SNAKE3_SNAKERENDERER_H
#define SNAKE3_SNAKERENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Snake.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"

using namespace Manager;

namespace Renderer {

    class SnakeRenderer : public BaseRenderer {
    public:
        SnakeRenderer(Snake *snake, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
        ~SnakeRenderer() override;
        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void toggleBlur();
        void toggleStyle(int style);

    protected:
        void renderScene(ShaderManager* shader);
        Snake* snake;
        Camera* camera;
        glm::mat4 projection;
        ShaderManager* baseShader;
        ShaderManager* shadowShader;
        ShaderManager* shaderLight;
        TextureManager* snakeTileTexture;
        TextureManager* snakeHeadTexture;
        ResourceManager* resourceManager;
        Mesh* mesh;
        bool blur;
        int renderStyle;
    };

} // Renderer

#endif //SNAKE3_SNAKERENDERER_H
