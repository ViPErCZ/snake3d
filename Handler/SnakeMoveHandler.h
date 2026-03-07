#ifndef SNAKE3_SNAKEMOVEHANDLER_H
#define SNAKE3_SNAKEMOVEHANDLER_H

#define UNIT_MOVE 0.125
#define VIRTUAL_MOVE 2 // kvuli nepresnosti float cislum pocitame virtualne v integer formatu
#define CUBE_SIZE 32

#include "BaseKeydownHandle.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"

using namespace ItemsDto;
using namespace Model;

namespace Handler {

    class SnakeMoveHandler final : public BaseKeydownHandle {
    public:
        ~SnakeMoveHandler() override;
        explicit SnakeMoveHandler(const shared_ptr<SnakeMeshNode3D> &snake_mesh_node);
        void onEventHandler(unsigned int key, int scancode, int action, int mods, float deltaTime) override;
        void onDefaultHandler() override;
        void addStartMoveCallback(const function<void()> &startMoveCallback);
        void setStopMoveCallback(const function<void(bool stop)> &stopMoveCallback);
        void setCrashCallback(const function<void()> &crashCallback);
        void setEatenUpCallback(const function<void()> &eatenUpCallback);

    protected:
        void changeMove(unsigned int direction);
        void stopMove();
        static void moveTile(const shared_ptr<SnakeMeshNode3D> &snakeMeshNode);
        template<typename Iter>
        [[nodiscard]] SnakeMeshNode3D::eDIRECTION findDirection(Iter iter) const;
        void createChangeCallback(unsigned int direction);
        [[nodiscard]] bool isChangeDirectionAllowed() const;
        [[nodiscard]] bool isNewDirectionCorrect(unsigned int direction) const;
        shared_ptr<SnakeMeshNode3D> snakeMeshNode;
        double lastTime{};
        double moveAccumulator{};
        double moveInterval = 0.1;
        bool stop;
        bool crashLock = false;
        bool eatenUpCallbackCalled;
        std::function<bool(shared_ptr<SnakeMeshNode3D>)> changeCallback;
        vector<std::function<void()>> startMoveCallbacks;
        std::function<void(bool stop)> stopMoveCallback;
        std::function<void()> crashCallback;
        std::function<void()> eatenUpCallback;
    };
} // Manager

#endif //SNAKE3_SNAKEMOVEHANDLER_H
