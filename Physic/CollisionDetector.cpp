#include "CollisionDetector.h"

namespace Physic {

    CollisionDetector::~CollisionDetector() {
        movingItems.clear();
        staticItems.clear();
    }

    void CollisionDetector::addStaticItem(const shared_ptr<BaseItem> &item) {
        staticItems.push_back(item);
    }

    void CollisionDetector::setPerimeter(const shared_ptr<ObjWall> &wall) {
        this->perimeter = wall;
    }

    void CollisionDetector::setBarriers(const shared_ptr<Barriers> &barriers) {
        CollisionDetector::barriers = barriers;
    }

    bool CollisionDetector::perimeterDetect(const shared_ptr<BaseItem> &snakeHead) const {

        if (perimeter == nullptr) {
            return false;
        }

        // for (auto Iter = perimeter->getItems().begin(); Iter < perimeter->getItems().end(); ++Iter) {
        //     blendBarrierDetect(snakeHead, Iter);
        // }

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

    bool CollisionDetector::detectWithStaticItem(const shared_ptr<BaseItem> &snakeHead) {
        for (auto Iter = staticItems.begin(); Iter < staticItems.end(); ++Iter) {
            if (!(*Iter)->isVisible()) {
                continue;
            }

            const int x = snakeHead->getVirtualX();
            const int y = snakeHead->getVirtualY();
            const int secondX = (*Iter)->getVirtualX();
            const int secondY = (*Iter)->getVirtualY();

            if (x - 16 + 32 >= secondX &&
                x - 16 <= secondX
                && y - 16 + 32 >= secondY &&
                y - 16 <= secondY) {
                return true;
            }
        }

        return false;
    }

    bool CollisionDetector::detect(const shared_ptr<BaseItem> &first, const shared_ptr<BaseItem> &second) {
        const int x = first->getVirtualX();
        const int y = first->getVirtualY();
        const int secondX = second->getVirtualX();
        const int secondY = second->getVirtualY();

        if (x - 16 + 32 > secondX && x - 16 <= secondX && y - 16 + 32 > secondY && y - 16 <= secondY) {
            return true;
        }

        return false;
    }

    bool CollisionDetector::barrierCollision(const shared_ptr<BaseItem> &snakeHead) const {
        if (barriers) {
            for (auto Iter = barriers->getItems().begin(); Iter < barriers->getItems().end(); ++Iter) {
                //blendBarrierDetect(snakeHead, Iter);
                if (detect(snakeHead, static_pointer_cast<BaseItem>(*Iter))) {
                    return true;
                }
            }
        }

        return false;
    }

    void CollisionDetector::blendBarrierDetect(shared_ptr<BaseItem> &snakeHead, shared_ptr<BaseItem> &barrier) {
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

    bool CollisionDetector::intoHimSelf(const shared_ptr<Snake> &snake) {
        const auto snakeHead = snake->getItems().begin()->get()->tile;

        for (auto Iter = snake->getItems().begin()+3; Iter < snake->getItems().end(); ++Iter) {
            if (!(*Iter)->tile->isVisible()) {
                continue;
            }

            if (detect(snakeHead, (*Iter)->tile)) {
                return true;
            }
        }

        return false;
    }

} // Physic