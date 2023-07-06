#ifndef SNAKE3_EATREMOVEANIMATERENDERER_H
#define SNAKE3_EATREMOVEANIMATERENDERER_H

#include "BaseRenderer.h"
#include "EatRenderer.h"
#include "../../ItemsDto/Eat.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class EatRemoveAnimateRenderer : public EatRenderer {
    public:
        explicit EatRemoveAnimateRenderer(Eat *eat, Camera *camera, const glm::mat4 &projection,
                                                       ResourceManager *resourceManager);
        ~EatRemoveAnimateRenderer() override;
        void render() override;
        void renderShadowMap() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        double lastTime{};
        glm::vec3 zoom{};
    };

} // Renderer

#endif //SNAKE3_EATREMOVEANIMATERENDERER_H
