#include "TextRenderer.h"
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

namespace Renderer {
    TextRenderer::TextRenderer(int width, int height) {
        this->width = width;
        this->height = height;
    }

    TextRenderer::~TextRenderer() {
        release();
    }

    void TextRenderer::render() {
        for (auto Iter = texts.begin(); Iter < texts.end(); Iter++) {
            if ((*Iter)->getText()->isVisible()) {
                (*Iter)->render();
            }
        }
    }

    void TextRenderer::beforeRender() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void TextRenderer::afterRender() {
        glDisable(GL_BLEND);
    }

    void TextRenderer::addText(ItemsDto::Text *text, ShaderManager* shader) {
        auto textModel = new Model::TextModel(width, height, shader);
        textModel->load(text);
        texts.push_back(textModel);
    }

    void TextRenderer::release() {
        for (auto Iter = texts.begin(); Iter < texts.end(); Iter++) {
            delete (*Iter);
        }

        texts.clear();
    }

} // Renderer