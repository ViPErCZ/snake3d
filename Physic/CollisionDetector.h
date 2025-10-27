#ifndef SNAKE3_COLLISIONDETECTOR_H
#define SNAKE3_COLLISIONDETECTOR_H

#include "../ItemsDto/BaseItem.h"
#include "../ItemsDto/Barriers.h"
#include "../ItemsDto/Snake.h"
#include "../ItemsDto/ObjWall.h"

using namespace ItemsDto;

namespace Physic {

    class CollisionDetector final {
    public:
        ~CollisionDetector();
        bool perimeterDetect(const shared_ptr<BaseItem> &snakeHead) const;
        bool detectWithStaticItem(const shared_ptr<BaseItem> &snakeHead);
        bool barrierCollision(const shared_ptr<BaseItem> &snakeHead) const;
        static bool intoHimSelf(const shared_ptr<Snake> &snake);
        static bool detect(const shared_ptr<BaseItem> &first, const shared_ptr<BaseItem> &second);
        void blendBarrierDetect(shared_ptr<BaseItem> &snakeHead, shared_ptr<BaseItem> &barrier);
        void setPerimeter(const shared_ptr<ObjWall> &wall);
        void setBarriers(const shared_ptr<Barriers> &barriers);
        void addStaticItem(const shared_ptr<BaseItem> &item);
    protected:
        vector<shared_ptr<BaseItem> > movingItems;
        shared_ptr<ObjWall> perimeter;
        shared_ptr<Barriers> barriers;
        vector<shared_ptr<BaseItem> > staticItems;
    };

} // Physic

#endif //SNAKE3_COLLISIONDETECTOR_H
