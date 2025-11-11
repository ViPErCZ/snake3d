#ifndef SNAKE3_RADARRENDERER_H
#define SNAKE3_RADARRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Radar.h"
#include "../../Manager/Camera.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include "Model/RadarModel.h"

using namespace Model;

namespace Renderer {
    class RadarRenderer final : public BaseRenderer {
    public:
        explicit RadarRenderer(shared_ptr<Radar> &radar, const shared_ptr<Camera> &camera,
                               const shared_ptr<ResourceManager> &resManager, const glm::mat4 &proj);

        void render3D(float dt) override;

        void beforeRender() override;

        void afterRender() override;

        void renderShadowMap() override {
        };

    protected:
        shared_ptr<Radar> radar;
        unique_ptr<RadarModel> model;
        shared_ptr<ResourceManager> resourceManager;
        shared_ptr<ShaderManager> shader;
        shared_ptr<TextureManager> frameTexture;
        shared_ptr<Camera> camera;
        glm::mat4 projection{};
    };
} // Renderer

#endif //SNAKE3_RADARRENDERER_H
