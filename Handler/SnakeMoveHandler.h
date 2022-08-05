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
        void onEventHandler(Uint32 key) override;
        void onDefaultHandler() override;
        void setCollisionDetector(CollisionDetector *collisionDetector);
        void setStartMoveCallback(const function<void(void)> &startMoveCallback);
        void setCrashCallback(const function<void()> &crashCallback);
        void setEatenUpCallback(const function<void()> &eatenUpCallback);

    protected:
        void ChangeMove(Uint32 direction);
        void StopMove();
        static eDIRECTION findDirection(sSNAKE_TILE* snakeTile, sSNAKE_TILE* mySelf);
        void createChangeCallback(Uint32 direction);
        static bool isChangeDirectionAllowed(sSNAKE_TILE* snake);
        static bool isNewDirectionCorrect(sSNAKE_TILE* headTile, Uint32 direction);
        Snake* snake;
        sSNAKE_TILE* snakeHead;
        CollisionDetector* collisionDetector{};
        Uint32 next_time{};
        bool stop;
        std::function<bool(sSNAKE_TILE*)> changeCallback;
        std::function<void()> startMoveCallback;
        std::function<void()> crashCallback;
        std::function<void()> eatenUpCallback;
    };

} // Manager

#endif //SNAKE3_SNAKEMOVEHANDLER_H
