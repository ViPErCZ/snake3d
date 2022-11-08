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
        [[nodiscard]] bool isCompleted() const;
        void setCompleted(bool completed);
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        double lastTime{};
        bool completed = false;
        glm::vec3 zoom{};
        float alpha;
    };

} // Renderer

#endif //SNAKE3_EATREMOVEANIMATERENDERER_H
