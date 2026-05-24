#ifndef SNAKE3_SNAKEMOVEHANDLER_H
#define SNAKE3_SNAKEMOVEHANDLER_H

#define UNIT_MOVE 0.125
#define VIRTUAL_MOVE 2 // kvuli nepresnosti float cislum pocitame virtualne v integer formatu
#define CUBE_SIZE 32

#include <memory>
#include <unordered_map>
#include <vector>

#include "Handler/BaseKeydownHandle.h"
#include "Physic/Jump/JumpTrajectory.h"
#include "../Renderer/Opengl/Model/Game/SnakeMeshNode3D.h"

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
        void setEnabled(bool enabled);
        void setStopped(bool stopped);
        void setInitialBodyDirection(SnakeMeshNode3D::eDIRECTION direction);
        void resetState();
        [[nodiscard]] bool isEnabled() const { return enabled; }
        [[nodiscard]] bool isStopped() const { return stop; }
        void stopMove();
        void tryStartJump();
        void setDynamicBody(const std::shared_ptr<Physic::Dynamics::DynamicBody> &body);
        void setFallDeathThreshold(const float z) { fallDeathThresholdZ = z; }
        // Predicate that answers "is there no floor at this snake-virtual cell?".
        // Used to drive the segment-by-segment fall when the snake crosses a hole edge.
        void setVoidPredicate(std::function<bool(int virtualX, int virtualY)> predicate) {
            voidPredicate = std::move(predicate);
        }

    protected:
        void changeMove(unsigned int direction);
        static void moveTile(const shared_ptr<SnakeMeshNode3D> &snakeMeshNode);
        template<typename Iter>
        [[nodiscard]] SnakeMeshNode3D::eDIRECTION findDirection(Iter iter) const;
        void createChangeCallback(unsigned int direction);
        [[nodiscard]] bool isChangeDirectionAllowed() const;
        [[nodiscard]] bool isNewDirectionCorrect(unsigned int direction) const;
        void startHeadJump();
        void claimBodyJumps();
        void advanceAllJumps(double dt);
        void clearAllJumps();
        [[nodiscard]] bool isHeadAirborne() const;
        // Chain-fall model with per-tile entry:
        //  * the head triggers chain mode the moment its center lands on a hole
        //    cell, whether via moveTile or at the end of a jump arc;
        //  * other tiles keep doing what they were doing - finishing arcs or
        //    walking toward the kink via moveTile - and each one joins the
        //    chain individually the moment it reaches the kink;
        //  * `chainProgress` advances at `chainVelocity`; once every tile has
        //    joined, gravity starts accelerating the chain ("drape" stays at
        //    the entry velocity so the chain feeds in without slowdowns).
        // Per-tile Z is `groundZ - (chainProgress - tileJoinProgress[tile])`,
        // so tiles that joined later are higher in the chain - exactly what a
        // chain dropping at uniform speed looks like.
        void checkAndStartHanging(const std::shared_ptr<SnakeMeshNode3D> &tile);
        void enterChainMode(float entryVelocity);
        void joinChain(SnakeMeshNode3D *tile);
        void updateChainMotion(double dt);
        void clearChainFall();
        [[nodiscard]] bool isInChain(const SnakeMeshNode3D *tile) const;
        [[nodiscard]] bool allTilesInChain() const;
        shared_ptr<SnakeMeshNode3D> snakeMeshNode;
        double lastTime{};
        double moveAccumulator{};
        double moveInterval = 0.1;
        bool stop;
        bool enabled = true;
        bool crashLock = false;
        bool eatenUpCallbackCalled;
        SnakeMeshNode3D::eDIRECTION initialBodyDirection = SnakeMeshNode3D::RIGHT;
        std::function<bool(shared_ptr<SnakeMeshNode3D>)> changeCallback;
        vector<std::function<void()>> startMoveCallbacks;
        std::function<void(bool stop)> stopMoveCallback;
        std::function<void()> crashCallback;
        std::function<void()> eatenUpCallback;
        struct TileJumpState {
            std::shared_ptr<Physic::Jump::JumpTrajectory> trajectory;
            double elapsed;
            float groundZ;
        };
        struct PendingTakeoff {
            int virtualX;
            int virtualY;
            int tilesRemaining;
            std::shared_ptr<Physic::Jump::JumpTrajectory> trajectory;
        };
        std::unordered_map<SnakeMeshNode3D *, TileJumpState> activeJumps;
        std::vector<PendingTakeoff> pendingTakeoffs;
        // Chain-mode state. Active from the moment the head's center first
        // reaches a void cell (whether on foot or at the end of a jump arc).
        bool chainActive = false;
        // Cumulative chain advance from chain start. Per-tile depth is
        // `chainProgress - tileJoinProgress[tile]`.
        float chainProgress = 0.0f;
        // Rate of chain advance (setPos/s, >= 0). Stays at the entry velocity
        // through the drape, accelerates after every tile has joined.
        float chainVelocity = 0.0f;
        // True once every snake tile has joined - from then on gravity kicks in.
        bool chainAccelerating = false;
        // The kink: where the chain bends. Sampled from head's position when
        // chain mode begins (the hole cell center the head is on).
        float chainKinkX = 0.0f;
        float chainKinkY = 0.0f;
        int chainKinkVirtualX = 0;
        int chainKinkVirtualY = 0;
        // Per-tile join progress: chainProgress at the moment the tile joined.
        // Tile's current depth past the kink is `chainProgress - joinProgress`.
        std::unordered_map<SnakeMeshNode3D *, float> tileJoinProgress;
        bool jumpRequested = false;
        std::shared_ptr<Physic::Dynamics::DynamicBody> dynamicBody;
        // The fall must last long enough for (a) every tile to drape over the
        // edge and (b) the camera to descend below the play plane so the player
        // sees the chain disappear. Threshold and gravity are tuned together -
        // see implementation comments.
        float fallDeathThresholdZ = -150.0f;
        std::function<bool(int virtualX, int virtualY)> voidPredicate;
    };
} // Manager

#endif //SNAKE3_SNAKEMOVEHANDLER_H
