#include "Wall.h"

namespace ItemsDto {
    Wall::~Wall() {
        Wall::release();
    }

    const vector<Cube *> &Wall::getItems() const {
        return walls;
    }

    void Wall::release() {
        if (!walls.empty()) {
            for (auto Iter = walls.begin(); Iter < walls.end(); Iter++) {
                delete (*Iter);
            }

            walls.clear();
        }
    }

    void Wall::init() {
        glm::vec3 wallPos = {-16, -16, 15};
        for (int round = 0; round < 2; round++) {
            for (int x = -16; x <= 1552; x += 32) {
                Cube *wall = new Cube();
                wallPos.x = (float) x;
                wall->addTexture(2);
                wall->setPosition(wallPos);
                wall->setZoom({15, 15, 15});
                wall->setVisible(true);
                wall->setWidth(512);
                wall->setHeight(512);
                wall->setVirtualX((((int)wallPos.x + (-23) / 2) * 32) + 16);
                wall->setVirtualX((((int)wallPos.y + (-23) / 2) * 32) + 16);

                walls.push_back(wall);
            }

            maxX = 1520;
            minX = 16;
            wallPos.x = -16;
            wallPos.y = (784 * 2) - 16;
        }

        wallPos.x = -16;
        wallPos.y = 16;
        minY = 16;
        maxY = (784 * 2) - 16 - 32;

        for (int round = 0; round < 2; round++) {
            for (int y = 16; y <= 1552; y += 32) {
                Cube *wall = new Cube();
                wallPos.y = (float) y;
                wall->addTexture(2);
                wall->setPosition(wallPos);
                wall->setVisible(true);
                wall->setZoom({15, 15, 15});
                wall->setWidth(512);
                wall->setHeight(512);
                wall->setVirtualX((((int)wallPos.x + (-23) / 2) * 32) + 16);
                wall->setVirtualX((((int)wallPos.y + (-23) / 2) * 32) + 16);

                this->walls.push_back(wall);
            }

            wallPos.x = 1552;
            wallPos.y = 16;
        }
    }

    int Wall::getMaxX() const {
        return maxX;
    }

    int Wall::getMaxY() const {
        return maxY;
    }

    int Wall::getMinX() const {
        return minX;
    }

    int Wall::getMinY() const {
        return minY;
    }

} // ItemsDto