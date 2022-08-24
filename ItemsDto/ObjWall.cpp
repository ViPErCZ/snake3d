#include "ObjWall.h"

namespace ItemsDto {
    ObjWall::~ObjWall() {
        ObjWall::release();
    }

    void ObjWall::release() {
        if (!walls.empty()) {
            for (auto Iter = walls.begin(); Iter < walls.end(); Iter++) {
                delete (*Iter);
            }

            walls.clear();
        }
    }

    const vector<ObjItem *> &ObjWall::getItems() const {
        return walls;
    }

    void ObjWall::init() {

//        auto *wall = new ObjItem();
//        wall->load("Assets/Objects/Cube.obj");
//        wall->addTexture(2);
//        wall->setPosition({23, 73, -23});
//        wall->setVisible(true);
//
//        walls.push_back(wall);

        glm::vec3 wallPos = {-16, -25, -23.0};
        for (int round = 0; round < 2; round++) {
            for (int x = -25; x <= 73; x += 2) {
                auto *wall = new ObjItem();
                wall->load("Assets/Objects/Cube.obj");
                wallPos.x = (float) x;
                wall->addTexture(2);
                wall->setPosition(wallPos);
                wall->setVisible(true);

                walls.push_back(wall);
            }

            maxX = 100;
            minX = 16;
            wallPos.x = -25;
            wallPos.y = 73;
        }

        wallPos.x = -25;
        wallPos.y = -25;
        minY = 16;
        maxY = (784 * 2) - 16 - 32;

        for (int round = 0; round < 2; round++) {
            for (int y = -25; y <= 73; y += 2) {
                auto wall = new ObjItem();
                wall->load("Assets/Objects/Cube.obj");
                wallPos.y = (float) y;
                wall->addTexture(2);
                wall->setPosition(wallPos);
                wall->setVisible(true);

                this->walls.push_back(wall);
            }

            wallPos.x = 73;
            wallPos.y = -25;
        }
    }

    int ObjWall::getMaxX() const {
        return 0;
    }

    int ObjWall::getMaxY() const {
        return 0;
    }

    int ObjWall::getMinX() const {
        return 0;
    }

    int ObjWall::getMinY() const {
        return 0;
    }
} // ItemsDto