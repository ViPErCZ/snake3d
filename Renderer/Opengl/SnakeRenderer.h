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
        SnakeRenderer(Snake *snake, ShaderManager *shader, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
        ~SnakeRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Snake* snake;
        ShaderManager* shader;
        Camera* camera;
        glm::mat4 projection;
        ResourceManager* resourceManager;
        Mesh* mesh;
        int tilesCounter;
    };

} // Renderer

#endif //SNAKE3_SNAKERENDERER_H
