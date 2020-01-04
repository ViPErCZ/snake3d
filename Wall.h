//
// Created by viper on 03.01.20.
//

#ifndef SNAKE_WALL_H
#define SNAKE_WALL_H

#include "CubeSprite.h"

class Wall : public CubeSprite {
public:
    Wall();
    ~Wall() override;
    void setPos(Vector2f pos);
};


#endif //SNAKE_WALL_H
