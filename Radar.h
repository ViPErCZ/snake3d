//
// Created by viper on 03.01.20.
//

#ifndef SNAKE_RADAR_H
#define SNAKE_RADAR_H

static const int SCREEN_EDGE = 18;

#include "PolygonSprite.h"

class Radar {
public:
    Radar();
    virtual ~Radar();
    void render(int width, int height);
    bool isVisibility() const;
    void setVisibility(bool visibility);
    const Vector3f &getEatPos() const;
    void setEatPos(const Vector3f &eatPos);
    void setSnakePos(const Vector3f &snakePos);
protected:
    void updatePositions();
    Vector3f computeCoords(const Vector3f &object) const;
protected:
    PolygonSprite* sprite;
    PolygonSprite* snake;
    PolygonSprite* eat;
    bool visibility;
    Vector3f eatPos;
    Vector3f snakePos;
};


#endif //SNAKE_RADAR_H
