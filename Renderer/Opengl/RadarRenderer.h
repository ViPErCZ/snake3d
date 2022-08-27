#ifndef SNAKE3_RADARRENDERER_H
#define SNAKE3_RADARRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Radar.h"
#include "Model/RadarModel.h"

using namespace Model;

namespace Renderer {

    class RadarRenderer : public BaseRenderer {
    public:
        explicit RadarRenderer(Radar* radar, ShaderManager* shader, Camera* camera, glm::mat4 proj, ResourceManager* resManager);
        ~RadarRenderer() override;

    public:
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Radar* radar;
        RadarModel* model;
        ResourceManager* resourceManager;
        ShaderManager* shader;
        Camera* camera;
        glm::mat4 projection{};
    };

} // Renderer

#endif //SNAKE3_RADARRENDERER_H
