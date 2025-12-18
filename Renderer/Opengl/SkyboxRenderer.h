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
        SkyboxRenderer(shared_ptr<Cube> cube, const shared_ptr<Camera> &camera, const glm::mat4 &projection,
                       const shared_ptr<ResourceManager> &resourceManager);

        void render3D(float dt) override;
        void beforeRender() override;
        void afterRender() override;
        void renderShadowMap() override {};
    protected:
        shared_ptr<Cube> cube;
        shared_ptr<Mesh> mesh;
        shared_ptr<Camera> camera;
        glm::mat4 projection;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ShaderManager> shader;
        shared_ptr<TextureManager> texture;
    };

} // Renderer

#endif //SNAKE3_SKYBOXRENDERER_H
