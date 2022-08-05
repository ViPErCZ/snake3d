#ifndef SNAKE3_EATRENDERER_H
#define SNAKE3_EATRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Eat.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class EatRenderer : public BaseRenderer {
    public:
        explicit EatRenderer(Eat *eat);
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Eat* eat;
        Uint32 lastTime{};
    };

} // Renderer

#endif //SNAKE3_EATRENDERER_H
