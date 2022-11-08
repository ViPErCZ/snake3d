#include "CollisionDetector.h"

namespace Physic {

    CollisionDetector::~CollisionDetector() {
        movingItems.clear();
        staticItems.clear();
    }

    void CollisionDetector::addStaticItem(BaseItem *item) {
        staticItems.push_back(item);
    }

    void CollisionDetector::setPerimeter(ObjWall *wall) {
        this->perimeter = wall;
    }

    void CollisionDetector::setBarriers(Barriers *barriers) {
        CollisionDetector::barriers = barriers;
    }

    bool CollisionDetector::perimeterDetect(BaseItem *snakeHead) {

        for (auto Iter = perimeter->getItems().begin(); Iter < perimeter->getItems().end(); Iter++) {
            blendBarrierDetect(snakeHead, (*Iter));
        }

        int x = snakeHead->getVirtualX();
        int y = snakeHead->getVirtualY();

        if (x > perimeter->getMaxX()
            || x < perimeter->getMinX()
            || y > perimeter->getMaxY()
            || y < perimeter->getMinY()
                ) { // detekujeme ohradu kolem hraciho pole
            return true;
        }

        return false;
    }

    bool CollisionDetector::detectWithStaticItem(BaseItem *snakeHead) {
        for (auto Iter = staticItems.begin(); Iter < staticItems.end(); Iter++) {
            if (!(*Iter)->isVisible()) {
                continue;
            }

            int x = snakeHead->getVirtualX();
            int y = snakeHead->getVirtualY();
            int secondX = (*Iter)->getVirtualX();
            int secondY = (*Iter)->getVirtualY();

            if (x - 16 + 32 >= secondX &&
                x - 16 <= secondX
                && y - 16 + 32 >= secondY &&
                y - 16 <= secondY) {
                return true;
            }
        }

        return false;
    }

    bool CollisionDetector::detect(BaseItem* first, BaseItem* second) {
        int x = first->getVirtualX();
        int y = first->getVirtualY();
        int secondX = second->getVirtualX();
        int secondY = second->getVirtualY();

        if (x - 16 + 32 > secondX && x - 16 <= secondX && y - 16 + 32 > secondY && y - 16 <= secondY) {
            return true;
        }

        return false;
    }

    bool CollisionDetector::barrierCollision(BaseItem *snakeHead) {
        for (auto Iter = barriers->getItems().begin(); Iter < barriers->getItems().end(); Iter++) {
            blendBarrierDetect(snakeHead, (*Iter));
            bool result = detect(snakeHead, (*Iter));
            if (result) {
                return true;
            }
        }

        return false;
    }

    void CollisionDetector::blendBarrierDetect(BaseItem *snakeHead, Cube *barrier) {
//        glm::vec3 headPosition = snakeHead->getPosition();
//        glm::vec3 barrierPosition = barrier->getPosition();
//
//        if (perimeter->getMaxX() + 32 > barrierPosition.x && perimeter->getMinX() - 32 < barrierPosition.x &&
//            barrierPosition.y <= headPosition.y - 32 && barrierPosition.y + 32 >= headPosition.y && (
//                headPosition.x >= barrierPosition.x - 128 && headPosition.x <= barrierPosition.x + 128)) {
//            barrier->setBlend(true);
//        } else {
//            barrier->setBlend(false);
//        }
    }

    bool CollisionDetector::intoHimSelf(Snake* snake) {
        auto snakeHead = (*snake->getItems().begin())->tile;

        for (auto Iter = snake->getItems().begin()+3; Iter < snake->getItems().end(); Iter++) {
            if (!(*Iter)->tile->isVisible()) {
                continue;
            }

            bool result = detect(snakeHead, (*Iter)->tile);
            if (result) {
                return true;
            }
        }

        return false;
    }

} // Physic