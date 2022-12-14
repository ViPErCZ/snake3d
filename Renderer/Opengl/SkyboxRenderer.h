#ifndef SNAKE3_SKYBOXRENDERER_H
#define SNAKE3_SKYBOXRENDERER_H

#include <map>
#include <glm/glm.hpp>
#include "BaseRenderer.h"
#include "../../ItemsDto/Cube.h"
#include "../../Manager/ShaderManager.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"
#include <vector>

using namespace std;
using namespace Manager;

namespace Renderer {

    class SkyboxRenderer : public BaseRenderer {
    public:
        SkyboxRenderer(Cube *cube, Camera *camera, const glm::mat4 &projection,
                       ResourceManager *resourceManager);

        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override {};
    protected:
        Cube* cube;
        Mesh* mesh;
        Camera* camera;
        glm::mat4 projection;
        ResourceManager* resourceManager;
        ShaderManager* shader;
        TextureManager* texture;
    };

} // Renderer

#endif //SNAKE3_SKYBOXRENDERER_H
