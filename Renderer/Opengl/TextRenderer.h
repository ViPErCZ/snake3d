#ifndef SNAKE3_TEXTRENDERER_H
#define SNAKE3_TEXTRENDERER_H

#include "BaseRenderer.h"
#include "../../ItemsDto/Text.h"

using namespace ItemsDto;

namespace Renderer {

    class TextRenderer : public BaseRenderer {
    public:
        TextRenderer();
        ~TextRenderer() override;
        void render() override;
        void beforeRender() override;
        void afterRender() override;
        void addText(Text* text);
    protected:
        void release();
        vector<Text*> texts;
    };

} // Renderer

#endif //SNAKE3_TEXTRENDERER_H
