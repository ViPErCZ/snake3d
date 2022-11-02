#ifndef SNAKE3_COLLISIONDETECTOR_H
#define SNAKE3_COLLISIONDETECTOR_H

#include "../ItemsDto/BaseItem.h"
#include "../ItemsDto/Barriers.h"
#include "../ItemsDto/BaseContainer.h"
#include "../ItemsDto/Snake.h"
#include "../ItemsDto/ObjWall.h"

using namespace ItemsDto;

namespace Physic {

    class CollisionDetector {
    public:
        virtual ~CollisionDetector();
        bool perimeterDetect(BaseItem* snakeHead);
        bool detectWithStaticItem(BaseItem* snakeHead);
        bool barrierCollision(BaseItem* snakeHead);
        static bool intoHimSelf(Snake* snake);
        static bool detect(BaseItem* first, BaseItem* second);
        void blendBarrierDetect(BaseItem* snakeHead, Cube* barrier);
        void setPerimeter(ObjWall* wall);
        void setBarriers(Barriers *barriers);
        void addStaticItem(BaseItem* item);
    protected:
        vector<BaseItem*> movingItems;
        ObjWall* perimeter;
        Barriers* barriers;
        vector<BaseItem*> staticItems;
    };

} // Physic

#endif //SNAKE3_COLLISIONDETECTOR_H
