#ifndef TORCHRENDERER_H
#define TORCHRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Cube.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

class TorchRenderer : public BaseRenderer {
public:
    TorchRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection, ResourceManager* resManager);
    ~TorchRenderer() override;
    void render(float dt) override;
    void renderShadowMap() override;
    void beforeRender() override;
    void afterRender() override;
protected:
    void renderScene(const ShaderManager* shader);
    Cube* cube;
    Mesh* mesh;
    Camera* camera;
    glm::mat4 projection;
    ResourceManager* resourceManager;
    ShaderManager* baseShader;
    TextureManager* texture;
    TextureManager* texture2;
};

} // Renderer

#endif //TORCHRENDERER_H
