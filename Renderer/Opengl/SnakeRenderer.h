#ifndef SNAKE3_SNAKERENDERER_H
#define SNAKE3_SNAKERENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Snake.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"

using namespace Manager;

namespace Renderer {

    class SnakeRenderer final : public BaseRenderer {
    public:
        SnakeRenderer(const shared_ptr<Snake> &snake, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
        void render(float dt) override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
        void toggleBlur();
        void toggleStyle(int style);

    protected:
        shared_ptr<Mesh> getMesh() override;
        void renderScene(const ShaderManager* shader) const;
        double startTime;
        shared_ptr<Snake> snake;
        Camera* camera;
        glm::mat4 projection;
        ShaderManager* baseShader;
        ShaderManager* shadowShader;
        ShaderManager* shaderLight;
        ShaderManager* respawn;
        TextureManager* snakeTileTexture;
        TextureManager* snakeHeadTexture;
        TextureManager* noise;
        ResourceManager* resourceManager;
        Mesh* mesh;
        bool blur;
        int renderStyle;
    };

} // Renderer

#endif //SNAKE3_SNAKERENDERER_H
