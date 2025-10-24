#ifndef SNAKE3_SNAKEMOVEHANDLER_H
#define SNAKE3_SNAKEMOVEHANDLER_H

#include "../ItemsDto/Snake.h"
#include "../Renderer/Opengl/Model/AnimationModel.h"
#include "../Physic/CollisionDetector.h"
#include "BaseKeydownHandle.h"

using namespace ItemsDto;
using namespace Physic;
using namespace Model;

namespace Handler {

    class SnakeMoveHandler : public BaseKeydownHandle {
    public:
        ~SnakeMoveHandler() override;
        explicit SnakeMoveHandler(const shared_ptr<Snake> &snake, const shared_ptr<AnimationModel> &animHead);
        void onEventHandler(unsigned int key, int scancode, int action, int mods) override;
        void onDefaultHandler() override;
        void setCollisionDetector(CollisionDetector *collisionDetector);
        void setStartMoveCallback(const function<void()> &startMoveCallback);
        void setCrashCallback(const function<void()> &crashCallback);
        void setEatenUpCallback(const function<void()> &eatenUpCallback);

    protected:
        void ChangeMove(unsigned int direction);
        void StopMove();
        static eDIRECTION findDirection(const shared_ptr<sSNAKE_TILE> &snakeTile, const shared_ptr<sSNAKE_TILE> &mySelf);
        void createChangeCallback(unsigned int direction);
        static bool isChangeDirectionAllowed(const shared_ptr<sSNAKE_TILE> &snake);
        static bool isNewDirectionCorrect(const shared_ptr<sSNAKE_TILE> &headTile, unsigned int direction);
        shared_ptr<Snake> snake;
        const shared_ptr<AnimationModel> animHead;
        shared_ptr<sSNAKE_TILE> snakeHead;
        CollisionDetector* collisionDetector{};
        double next_time{};
        bool stop;
        bool eatenUpCallbackCalled;
        std::function<bool(shared_ptr<sSNAKE_TILE>)> changeCallback;
        std::function<void()> startMoveCallback;
        std::function<void()> crashCallback;
        std::function<void()> eatenUpCallback;
    };

} // Manager

#endif //SNAKE3_SNAKEMOVEHANDLER_H
