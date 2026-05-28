#ifndef SNAKE3_LAYERS_H
#define SNAKE3_LAYERS_H

enum Layers {
    WORLD = 1,
    PLAYER = 2,
    PLAYER_BODY = 4,
    ENEMY = 8,
    ENEMY_BODY = 16,
    FLOOR = 32,
    //PROJECTILE = 64
};

#endif //SNAKE3_LAYERS_H
