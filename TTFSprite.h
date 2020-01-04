#ifndef SNAKE_TTFSPRITE_H
#define SNAKE_TTFSPRITE_H

#include "Sprite.h"
#include <GL/glu.h>
#include <GL/glut.h>
#include <SDL2/SDL_ttf.h>
#include <string>


class TTFSprite:
        public Sprite {

public:
    explicit TTFSprite(SDL_Renderer * renderer, string str, SDL_Color color);
    virtual ~TTFSprite();
    void Render() override;

protected:
    string text;
    SDL_Surface * surface;
    SDL_Texture * texture;
    SDL_Renderer * renderer;

};


#endif //SNAKE_TTFSPRITE_H
