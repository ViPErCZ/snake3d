#include "TextRenderer.h"

namespace Renderer {
    TextRenderer::TextRenderer() {
        ortho = true;
    }

    TextRenderer::~TextRenderer() {
        release();
    }

    void TextRenderer::render() {
        for (auto Iter = texts.begin(); Iter < texts.end(); Iter++) {
            if ((*Iter)->isVisible()) {
                sCOLOR color = (*Iter)->getColor();
                glColor3f(color.r, color.g, color.b);
                glRasterPos2i((GLint) (*Iter)->getPosition().x, (GLint) (*Iter)->getPosition().y);
                //glRasterPos2i(3, 15);
                const char *str = (*Iter)->getText().c_str();
                while (*str) {
                    glutBitmapCharacter((*Iter)->getFont(), *str++);
                }
            }
        }
    }

    void TextRenderer::beforeRender() {
// ------------------------------------------------------
        glDisable(GL_TEXTURE_2D);
        float LightPos[4] = {-5.0f, 5.0f, 10.0f, 1.0f};
        float Ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        float Ambient2[4] = {0.0f, 0.2f, 0.9f, 10.5f};;
        glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
    }

    void TextRenderer::afterRender() {
        glPopAttrib();
        glPopMatrix();
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    void TextRenderer::addText(Text *text) {
        texts.push_back(text);
    }

    void TextRenderer::release() {
        for (auto Iter = texts.begin(); Iter < texts.end(); Iter++) {
            delete (*Iter);
        }

        texts.clear();
    }

} // Renderer