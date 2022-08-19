#ifndef SNAKE3_TEXTRENDERER_H
#define SNAKE3_TEXTRENDERER_H

#include <map>
#include <glm/glm.hpp>
#include "BaseRenderer.h"
#include "../../ItemsDto/Text.h"
#include "../../Manager/ShaderManager.h"
#include "Model/TextModel.h"

using namespace Manager;

namespace Renderer {

    class TextRenderer : public BaseRenderer {
    public:
        TextRenderer(int width, int height);
        ~TextRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void addText(ItemsDto::Text* text, ShaderManager* shader);
    protected:
        void release();
        vector<Model::TextModel*> texts;
        int width;
        int height;
    };

} // Renderer

#endif //SNAKE3_TEXTRENDERER_H
