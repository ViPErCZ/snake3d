#ifndef SNAKE3_GAMEFIELDRENDERER_H
#define SNAKE3_GAMEFIELDRENDERER_H

#include "../../ItemsDto/GameField.h"
#include "Model/GameFieldModel.h"
#include "BaseRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ItemsDto;
using namespace Model;

namespace Renderer {
    class GameFieldRenderer : public BaseRenderer {
    public:
        explicit GameFieldRenderer(GameField *item, ShaderManager* shader, Camera* camera, glm::mat4 proj);
        ~GameFieldRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
    protected:
        GameField* gameField;
        vector<GameFieldModel*> model;
        ShaderManager* shader;
        Camera* camera;
        glm::mat4 projection{};
    };
}

#endif //SNAKE3_GAMEFIELDRENDERER_H
