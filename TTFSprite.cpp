//
// Created by viper on 31.12.18.
//

#include "TTFSprite.h"

TTFSprite::TTFSprite(SDL_Renderer * renderer, string str, SDL_Color color) {
    text = std::move(str);
    TTF_Font * font = TTF_OpenFont("arial.ttf", 25);
    this->surface = TTF_RenderText_Solid(font, text.c_str(), color);
    this->texture = SDL_CreateTextureFromSurface(renderer, this->surface);
    this->renderer = renderer;
}

TTFSprite::~TTFSprite() {
    SDL_DestroyTexture(this->texture);
    TTF_Quit();
}

void TTFSprite::Render() {
    Sprite::Render();

    int iW, iH;
    SDL_QueryTexture(this->texture, NULL, NULL, &iW, &iH);
    SDL_RenderClear(renderer);
    int x = 1024 / 2 - iW / 2;
    int y = 768 / 2 - iH / 2;
    SDL_Rect rect = { 0, 0, x, y };
    SDL_RenderCopy(this->renderer, this->texture, nullptr, &rect);
    SDL_RenderPresent(this->renderer);
}
