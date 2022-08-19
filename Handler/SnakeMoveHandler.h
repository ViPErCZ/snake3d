#ifndef SNAKE3_SNAKEMOVEHANDLER_H
#define SNAKE3_SNAKEMOVEHANDLER_H

#include "../ItemsDto/Snake.h"
#include "../Physic/CollisionDetector.h"
#include "BaseKeydownHandle.h"

using namespace ItemsDto;
using namespace Physic;

namespace Handler {

    class SnakeMoveHandler : public BaseKeydownHandle {
    public:
        ~SnakeMoveHandler() override;
        explicit SnakeMoveHandler(Snake *snake);
        void onEventHandler(unsigned int key) override;
        void onDefaultHandler() override;
        void setCollisionDetector(CollisionDetector *collisionDetector);
        void setStartMoveCallback(const function<void(void)> &startMoveCallback);
        void setCrashCallback(const function<void()> &crashCallback);
        void setEatenUpCallback(const function<void()> &eatenUpCallback);

    protected:
        void ChangeMove(unsigned int direction);
        void StopMove();
        static eDIRECTION findDirection(sSNAKE_TILE* snakeTile, sSNAKE_TILE* mySelf);
        void createChangeCallback(unsigned int direction);
        static bool isChangeDirectionAllowed(sSNAKE_TILE* snake);
        static bool isNewDirectionCorrect(sSNAKE_TILE* headTile, unsigned int direction);
        Snake* snake;
        sSNAKE_TILE* snakeHead;
        CollisionDetector* collisionDetector{};
        double next_time{};
        bool stop;
        std::function<bool(sSNAKE_TILE*)> changeCallback;
        std::function<void()> startMoveCallback;
        std::function<void()> crashCallback;
        std::function<void()> eatenUpCallback;
    };

} // Manager

#endif //SNAKE3_SNAKEMOVEHANDLER_H
