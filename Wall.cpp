//

#include "Wall.h"

Wall::Wall() = default;

Wall::~Wall() = default;

void Wall::setPos(Vector2f pos) {
    Vector3f posSprite = {pos.x, pos.y, 15};

    CubeSprite::setPos(posSprite);
}
