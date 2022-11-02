#ifndef SNAKE3_EATRENDERER_H
#define SNAKE3_EATRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Eat.h"
#include "../../Manager/ResourceManager.h"
#include "../../Manager/ShaderManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace ItemsDto;
using namespace Manager;

namespace Renderer {

    class EatRenderer : public BaseRenderer {
    public:
        EatRenderer(Eat *eat, ShaderManager *shaderManager, Camera *camera, const glm::mat4 &projection,
                    ResourceManager *resourceManager);

        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        Eat* eat;
        Mesh* mesh;
        ShaderManager* shaderManager;
        Camera* camera;
        glm::mat4 projection{};
        ResourceManager* resourceManager;
        double lastTime{};
    };

} // Renderer

#endif //SNAKE3_EATRENDERER_H
