#ifndef SNAKE3_COLLISIONDETECTOR_H
#define SNAKE3_COLLISIONDETECTOR_H

#include "../ItemsDto/BaseItem.h"
#include "../ItemsDto/Barriers.h"
#include "../ItemsDto/BaseContainer.h"
#include "../ItemsDto/Wall.h"
#include "../ItemsDto/Snake.h"

using namespace ItemsDto;

namespace Physic {

    class CollisionDetector {
    public:
        virtual ~CollisionDetector();
        bool perimeterDetect(BaseItem* snakeHead);
        bool detectWithStaticItem(BaseItem* snakeHead);
        bool barrierCollision(BaseItem* snakeHead);
        bool intoHimSelf(Snake* snake);
        static bool detect(BaseItem* first, BaseItem* second);
        void blendBarrierDetect(BaseItem* snakeHead, BaseItem* barrier);
        void setPerimeter(Wall* wall);
        void setBarriers(Barriers *barriers);
        void addStaticItem(BaseItem* item);
    protected:
        vector<BaseItem*> movingItems;
        Wall* perimeter;
        Barriers* barriers;
        vector<BaseItem*> staticItems;
    };

} // Physic

#endif //SNAKE3_COLLISIONDETECTOR_H