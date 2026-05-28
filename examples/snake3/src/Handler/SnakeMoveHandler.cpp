#include "SnakeMoveHandler.h"

#include "Physic/Dynamics/DynamicBody.h"

using namespace Build;
using namespace Physic;
using namespace Physic::Dynamics;

namespace Handler {
    namespace {
        // Gravity acceleration (setPos/s^2) applied to the chain throughout its
        // motion. Real gravity at the snake's 0.041667 scale is ~235 setPos/s^2,
        // which is too fast to watch; ~42% gives a few seconds of visible
        // acceleration from the entry velocity.
        constexpr float kChainGravity = 100.0f;
        // Chain segment spacing in setPos. 1 grid cell = CUBE_SIZE virtual
        // units = CUBE_SIZE / VIRTUAL_MOVE moveTiles * UNIT_MOVE.
        constexpr float kChainCellSize =
            static_cast<float>(CUBE_SIZE) / static_cast<float>(VIRTUAL_MOVE) * static_cast<float>(UNIT_MOVE);
        // Snake's resting Z in setPos units (where the floor places the tile
        // sphere centers). Used as the kink's Z reference.
        constexpr float kGroundZ = -23.0f;
    }

    SnakeMoveHandler::SnakeMoveHandler(const shared_ptr<SnakeMeshNode3D> &snake_mesh_node)
        : snakeMeshNode(snake_mesh_node), eatenUpCallbackCalled(false) {
        stop = false;
        lastTime = glfwGetTime();
        moveAccumulator = 0.0;
        constexpr double stepsPerTile = static_cast<double>(CUBE_SIZE) / static_cast<double>(VIRTUAL_MOVE);
        constexpr double tilesPerSecond = 3.5;
        moveInterval = 1.0 / (tilesPerSecond * stepsPerTile);
    }

    void SnakeMoveHandler::setDynamicBody(const std::shared_ptr<DynamicBody> &body) {
        // The chain-fall handler owns all vertical motion of the snake once a
        // tile crosses the edge, so the DynamicBody is only used as a hook to
        // freeze the head while we drive it ourselves (see onDefaultHandler).
        dynamicBody = body;
    }

    SnakeMoveHandler::~SnakeMoveHandler() = default;

    void SnakeMoveHandler::onEventHandler(const unsigned int key, int scancode, const int action, int mods, float deltaTime) {
        if (!enabled) {
            return;
        }
        switch (key) {
            case GLFW_KEY_L:
            case GLFW_KEY_I:
            case GLFW_KEY_J:
            case GLFW_KEY_K:
                if (!stop && snakeMeshNode->isReady()) {
                    changeMove(key);
                }
                break;
            case GLFW_KEY_PAUSE:
                stopMove();
                break;
            case GLFW_KEY_SPACE:
                if (!stop && snakeMeshNode->isReady()) {
                    tryStartJump();
                }
                break;
            default:
                break;
        }
    }

    void SnakeMoveHandler::stopMove() {
        if (!changeCallback) {
            stop = !stop;
            stopMoveCallback(stop);
        }
    }

    void SnakeMoveHandler::tryStartJump() {
        if (jumpRequested || isHeadAirborne() || chainActive) {
            return;
        }
        const auto dir = snakeMeshNode->getDirection();
        if (dir != SnakeMeshNode3D::LEFT && dir != SnakeMeshNode3D::RIGHT &&
            dir != SnakeMeshNode3D::UP && dir != SnakeMeshNode3D::DOWN) {
            return;
        }
        // Fires once the head reaches the next cell center - same gate as direction change.
        jumpRequested = true;
    }

    bool SnakeMoveHandler::isHeadAirborne() const {
        return activeJumps.contains(snakeMeshNode.get());
    }

    void SnakeMoveHandler::startHeadJump() {
        glm::vec3 directionVector;
        switch (snakeMeshNode->getDirection()) {
            case SnakeMeshNode3D::LEFT:  directionVector = {-1.0f, 0.0f, 0.0f}; break;
            case SnakeMeshNode3D::RIGHT: directionVector = { 1.0f, 0.0f, 0.0f}; break;
            case SnakeMeshNode3D::UP:    directionVector = { 0.0f, 1.0f, 0.0f}; break;
            case SnakeMeshNode3D::DOWN:  directionVector = { 0.0f,-1.0f, 0.0f}; break;
            default:
                jumpRequested = false;
                return;
        }
        constexpr float tileWorldUnits = static_cast<float>(CUBE_SIZE) * static_cast<float>(UNIT_MOVE)
                                         / static_cast<float>(VIRTUAL_MOVE);
        // Skip one cell: peak above the next cell center, land at the cell after.
        constexpr float distance = 2.0f * tileWorldUnits;
        constexpr float peakHeight = 1.5f * tileWorldUnits;
        constexpr double stepsPerTile = static_cast<double>(CUBE_SIZE) / static_cast<double>(VIRTUAL_MOVE);
        const auto duration = static_cast<float>(2.0 * stepsPerTile * moveInterval);

        const auto headPos = snakeMeshNode->getPosition();
        const glm::vec3 endPos = headPos + directionVector * distance;
        const auto trajectory = std::make_shared<Jump::JumpTrajectory>(headPos, endPos, duration, peakHeight);

        // Body tiles will claim a slot as they each visit this exact cell.
        PendingTakeoff event{};
        event.virtualX = snakeMeshNode->x;
        event.virtualY = snakeMeshNode->y;
        event.tilesRemaining = static_cast<int>(snakeMeshNode->getChildren().size());
        event.trajectory = trajectory;
        if (event.tilesRemaining > 0) {
            pendingTakeoffs.push_back(event);
        }

        activeJumps[snakeMeshNode.get()] = TileJumpState{trajectory, 0.0, headPos.z};
        jumpRequested = false;
        // Jump owns the head's vertical motion - park gravity until the head lands.
        if (dynamicBody) {
            dynamicBody->setUseGravity(false);
            dynamicBody->setVelocity({0.0f, 0.0f, 0.0f});
        }
    }

    void SnakeMoveHandler::claimBodyJumps() {
        if (pendingTakeoffs.empty()) {
            return;
        }
        for (const auto &child : snakeMeshNode->getChildren()) {
            const auto bodyTile = dynamic_pointer_cast<SnakeMeshNode3D>(child);
            if (!bodyTile) {
                continue;
            }
            if (activeJumps.contains(bodyTile.get())) {
                continue; // already arcing
            }
            // First pending event matching this tile's exact cell wins (FIFO).
            for (auto & pendingTakeoff : pendingTakeoffs) {
                if (pendingTakeoff.tilesRemaining <= 0) {
                    continue;
                }
                if (bodyTile->x == pendingTakeoff.virtualX && bodyTile->y == pendingTakeoff.virtualY) {
                    activeJumps[bodyTile.get()] = TileJumpState{pendingTakeoff.trajectory, 0.0, bodyTile->getPosition().z};
                    pendingTakeoff.tilesRemaining -= 1;
                    break;
                }
            }
        }
        std::erase_if(pendingTakeoffs, [](const PendingTakeoff &e) { return e.tilesRemaining <= 0; });
    }

    void SnakeMoveHandler::advanceAllJumps(const double dt) {
        for (auto it = activeJumps.begin(); it != activeJumps.end();) {
            auto *tile = it->first;
            auto &state = it->second;
            state.elapsed += dt;
            const auto t = static_cast<float>(state.elapsed);
            const auto trajPos = state.trajectory->positionAt(t);
            // Only the vertical (up) axis is overridden; horizontal motion stays driven by moveTile.
            auto pos = tile->getPosition();
            pos.z = state.groundZ + (trajPos.z - state.trajectory->positionAt(0.0f).z);
            tile->setPosition(pos);
            if (state.trajectory->isFinished(t)) {
                pos.z = state.groundZ;
                tile->setPosition(pos);
                const bool wasHead = (tile == snakeMeshNode.get());
                it = activeJumps.erase(it);
                // Once the head lands, hand the vertical axis back to gravity.
                if (wasHead && dynamicBody) {
                    dynamicBody->setUseGravity(true);
                    dynamicBody->setVelocity({0.0f, 0.0f, 0.0f});
                }
            } else {
                ++it;
            }
        }
    }

    void SnakeMoveHandler::clearAllJumps() {
        for (auto &[tile, state] : activeJumps) {
            auto pos = tile->getPosition();
            pos.z = state.groundZ;
            tile->setPosition(pos);
        }
        activeJumps.clear();
        pendingTakeoffs.clear();
        jumpRequested = false;
        clearChainFall();
    }

    void SnakeMoveHandler::clearChainFall() {
        chainActive = false;
        chainProgress = 0.0f;
        chainVelocity = 0.0f;
        chainAccelerating = false;
        chainKinkX = 0.0f;
        chainKinkY = 0.0f;
        chainKinkVirtualX = 0;
        chainKinkVirtualY = 0;
        tileJoinProgress.clear();
    }

    bool SnakeMoveHandler::isInChain(const SnakeMeshNode3D *tile) const {
        if (!tile) return false;
        return tileJoinProgress.contains(const_cast<SnakeMeshNode3D *>(tile));
    }

    bool SnakeMoveHandler::allTilesInChain() const {
        if (!isInChain(snakeMeshNode.get())) return false;
        for (const auto &child : snakeMeshNode->getChildren()) {
            const auto tile = dynamic_cast<SnakeMeshNode3D *>(child.get());
            if (!tile) continue;
            if (!isInChain(tile)) return false;
        }
        return true;
    }

    void SnakeMoveHandler::joinChain(SnakeMeshNode3D *tile) {
        if (!tile || isInChain(tile)) return;
        tileJoinProgress[tile] = chainProgress;
        glm::vec3 pos = tile->getPosition();
        pos.x = chainKinkX;
        pos.y = chainKinkY;
        pos.z = kGroundZ;
        tile->setPosition(pos);
    }

    void SnakeMoveHandler::enterChainMode(const float entryVelocity) {
        if (chainActive) return;
        chainActive = true;
        chainProgress = 0.0f;
        chainVelocity = entryVelocity;
        chainAccelerating = false;
        const auto headPos = snakeMeshNode->getPosition();
        chainKinkX = headPos.x;
        chainKinkY = headPos.y;
        chainKinkVirtualX = snakeMeshNode->x;
        chainKinkVirtualY = snakeMeshNode->y;
        // Drop any pending input - chain mode owns the snake from here, and the
        // queued callback would otherwise sit waiting until after respawn.
        changeCallback = nullptr;
        jumpRequested = false;
        if (dynamicBody) {
            dynamicBody->setVelocity({0.0f, 0.0f, 0.0f});
            dynamicBody->setEnabled(false);
        }
        // Tilt the head 90 degrees forward so it looks down into the hole. Rz
        // is applied first (Rz then Ry then Rx in glm::rotate order), so adding
        // Rz=-90 effectively rotates the mesh around its lateral axis whatever
        // the snake was heading - i.e. the head "noses over" the edge while
        // its top stays aligned with the original direction of motion.
        snakeMeshNode->setRotationZ(-90);
        // Only the head joins immediately. Body tiles keep doing what they were
        // doing (finishing arcs, walking toward the kink) and each joins on its
        // own terms - that's what makes the chain stay together through a jump
        // into the hole.
        joinChain(snakeMeshNode.get());
    }

    void SnakeMoveHandler::updateChainMotion(const double dt) {
        if (!chainActive || dt <= 0.0) return;
        const auto dtf = static_cast<float>(dt);
        // Symplectic Euler: velocity first, then position. Gravity kicks in
        // only after every tile has joined - until then the chain feeds in at
        // a steady "entry velocity" so it stays rigid (head depth grows at the
        // same rate the still-moving tail advances toward the kink).
        if (chainAccelerating) {
            chainVelocity += kChainGravity * dtf;
        }
        chainProgress += chainVelocity * dtf;

        for (auto &[tile, joinProgress] : tileJoinProgress) {
            glm::vec3 pos = tile->getPosition();
            pos.z = kGroundZ - (chainProgress - joinProgress);
            tile->setPosition(pos);
        }
    }

    void SnakeMoveHandler::checkAndStartHanging(const shared_ptr<SnakeMeshNode3D> &tile) {
        if (!tile || !voidPredicate) return;
        if (chainActive) return;
        // Only the head triggers chain mode - the body follows the chain. This
        // avoids spurious mid-snake triggers if body tiles happen to coincide
        // with a void cell during odd geometries.
        if (tile != snakeMeshNode) return;
        if (activeJumps.contains(tile.get())) return;
        // Cell centers are at (gridX*32 + 16, gridY*32 + 16); imagine a cube
        // tipping over: it only fits into the hole once the whole footprint is
        // over it, so we transition exactly at the center.
        if ((tile->x - 16) % CUBE_SIZE != 0) return;
        if ((tile->y - 16) % CUBE_SIZE != 0) return;
        if (voidPredicate(tile->x, tile->y)) {
            // Normal entry: chain inherits the snake's forward speed so there's
            // no abrupt change at the moment of transition.
            const auto entryVelocity = static_cast<float>(UNIT_MOVE / moveInterval);
            enterChainMode(entryVelocity);
        }
    }

    void SnakeMoveHandler::moveTile(const shared_ptr<SnakeMeshNode3D> &snakeMeshNode) {
        glm::vec3 pos = snakeMeshNode->getPosition();
        switch (snakeMeshNode->getDirection()) {
            case SnakeMeshNode3D::LEFT:
                pos.x -= UNIT_MOVE;
                snakeMeshNode->x = snakeMeshNode->x - VIRTUAL_MOVE;
                break;
            case SnakeMeshNode3D::RIGHT:
                pos.x += UNIT_MOVE;
                snakeMeshNode->x = snakeMeshNode->x + VIRTUAL_MOVE;
                break;
            case SnakeMeshNode3D::UP:
                pos.y += UNIT_MOVE;
                snakeMeshNode->y = snakeMeshNode->y + VIRTUAL_MOVE;
                break;
            case SnakeMeshNode3D::DOWN:
                pos.y -= UNIT_MOVE;
                snakeMeshNode->y = snakeMeshNode->y - VIRTUAL_MOVE;
                break;
            case SnakeMeshNode3D::CRASH:
            case SnakeMeshNode3D::PAUSE:
            case SnakeMeshNode3D::STOP:
            case SnakeMeshNode3D::NONE:
                break;
        }
        snakeMeshNode->setPosition(pos);
    }

    void SnakeMoveHandler::changeMove(const unsigned int direction) {
        if (changeCallback ||
            isHeadAirborne() ||
            chainActive ||
            !isNewDirectionCorrect(direction)) {
            return;
        }

        createChangeCallback(direction);
    }

    void SnakeMoveHandler::createChangeCallback(unsigned int direction) {
        changeCallback = [this, direction](const shared_ptr<SnakeMeshNode3D> &snakeMeshNode) {
            if (isChangeDirectionAllowed()) {
                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::NONE && !startMoveCallbacks.empty()) {
                    // start hry
                    for (auto &startMoveCallback: startMoveCallbacks) {
                        startMoveCallback();
                    }
                }

                switch (direction) {
                    case GLFW_KEY_J: // left
                        snakeMeshNode->setDirection(SnakeMeshNode3D::LEFT);
                        snakeMeshNode->setRotationX(90);
                        snakeMeshNode->setRotationY(180);
                        break;
                    case GLFW_KEY_L: // right
                        snakeMeshNode->setDirection(SnakeMeshNode3D::RIGHT);
                        snakeMeshNode->setRotationX(90);
                        snakeMeshNode->setRotationY(0);
                        break;
                    case GLFW_KEY_I:
                        snakeMeshNode->setDirection(SnakeMeshNode3D::UP);
                        snakeMeshNode->setRotationX(90);
                        snakeMeshNode->setRotationY(90);
                        break;
                    case GLFW_KEY_K:
                        snakeMeshNode->setDirection(SnakeMeshNode3D::DOWN);
                        snakeMeshNode->setRotationX(90);
                        snakeMeshNode->setRotationY(-90);
                        break;
                    default:
                        break;
                }

                for (auto &node: snakeMeshNode->getChildren()) {
                    // Initial body direction depends on the spawn layout.
                    const auto snakeMesh = dynamic_pointer_cast<SnakeMeshNode3D>(node);
                    const auto nodeDirection = snakeMesh->getDirection();
                    if (nodeDirection == SnakeMeshNode3D::NONE) {
                        snakeMesh->setDirection(initialBodyDirection);
                    }
                }

                return true;
            }

            return false;
        };
    }

    void SnakeMoveHandler::onDefaultHandler() {
        if (!enabled) {
            lastTime = glfwGetTime();
            return;
        }

        const double now = glfwGetTime();
        const double deltaTime = now - lastTime;
        lastTime = now;

        if (crashLock && snakeMeshNode->isReady()) {
            crashLock = false;
            // Drop any input that was queued before or during the crash flow.
            // Otherwise an arrow pressed seconds ago - just before the snake
            // fell into a hole - would auto-start the respawned snake the
            // instant isReady() flips back to true.
            changeCallback = nullptr;
            jumpRequested = false;
        }

        // Gravity is only allowed when the snake is fully alive (isReady() =
        // normal material AND no respawn timer). While the chain controls the
        // snake's motion, the DynamicBody is frozen even though the snake is
        // technically alive.
        if (dynamicBody) {
            const bool alive = !crashLock && snakeMeshNode->isReady();
            if (!alive || chainActive) {
                dynamicBody->setVelocity({0.0f, 0.0f, 0.0f});
                dynamicBody->setEnabled(false);
            } else if (!dynamicBody->isEnabled()) {
                dynamicBody->setVelocity({0.0f, 0.0f, 0.0f});
                dynamicBody->setEnabled(true);
            }
        }

        // Fell off the world (through a hole / off the board edge): trigger crash
        // once, then let the existing crash/respawn flow take over.
        if (!crashLock && crashCallback &&
            snakeMeshNode->getPosition().z < fallDeathThresholdZ) {
            clearAllJumps();
            crashLock = true;
            crashCallback();
            return;
        }

        if (snakeMeshNode->isCrashing()) {
            moveAccumulator = 0;
            lastTime = glfwGetTime();
            clearAllJumps();
            return;
        }

        // Block the move-tick loop while the snake is mid-respawn. isCrashing
        // covers the crash-explosion window; the respawn shader runs after
        // that with the timer still active, during which isReady() is false.
        // Without this gate the inner loop would fire a held changeCallback
        // mid-respawn-animation. Chain mode keeps isReady() == true (the
        // snake is alive), so the chain fall is unaffected.
        if (!snakeMeshNode->isReady()) {
            moveAccumulator = 0;
            lastTime = glfwGetTime();
            return;
        }

        if (stop) {
            return;
        }

        // Chain motion progresses every frame; the move-tick loop below also
        // keeps running so that body tiles still in an arc (or walking on the
        // table) can finish their journey and join the chain in their own time.
        if (chainActive) {
            updateChainMotion(deltaTime);
        }

        moveAccumulator += deltaTime;

        while (moveAccumulator >= moveInterval) {
            moveAccumulator -= moveInterval;

            // No steering or direction-NONE shortcut while the chain is active:
            // the snake has lost its rudder, and we still want to keep ticking
            // so mid-flight body tiles can land at the kink and join.
            if (!chainActive) {
                if (changeCallback) {
                    if (changeCallback(snakeMeshNode)) {
                        changeCallback = nullptr;
                    }
                }

                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::NONE ||
                    snakeMeshNode->getDirection() == SnakeMeshNode3D::STOP) {
                    moveAccumulator = 0;
                    return;
                }
            }

            // `headAllowed` gates head-side decisions (steering, new jumps).
            // Body-tile direction updates are gated per-tile below: in chain
            // mode the head is locked at a cell center, so the head-based
            // check would either always pass or be force-disabled - neither is
            // right. Each body tile must check its OWN cell-center crossing so
            // that tiles approaching the hole keep turning along the head's
            // path (and claim pending jumps when they reach the takeoff cell).
            const bool headAllowed = !chainActive && isChangeDirectionAllowed();
            const auto &children = snakeMeshNode->getChildren();

            for (auto Iter = children.rbegin(); Iter != children.rend(); ++Iter) {
                const auto &tile = dynamic_pointer_cast<SnakeMeshNode3D>(*Iter);
                if (!tile) continue;
                if (isInChain(tile.get())) continue;  // chain owns this tile's position
                const bool tileAtCenter =
                    ((tile->x - 16) % CUBE_SIZE == 0) && ((tile->y - 16) % CUBE_SIZE == 0);
                if (tileAtCenter) {
                    tile->setDirection(findDirection(std::next(Iter).base()));
                }
                moveTile(tile);
                // Normal-entry case: while the chain is already active, the
                // tile is walking on the table toward the kink. Once it lands
                // exactly on the kink cell, it joins the chain.
                if (chainActive &&
                    tile->x == chainKinkVirtualX && tile->y == chainKinkVirtualY) {
                    joinChain(tile.get());
                }
            }

            if (!isInChain(snakeMeshNode.get())) {
                if (headAllowed && jumpRequested && !isHeadAirborne()) {
                    startHeadJump();
                }
                moveTile(snakeMeshNode);
                checkAndStartHanging(snakeMeshNode);
            }

            // Snapshot every tile's pre-advance arc state so we can detect arcs
            // that finish in this exact tick.
            const auto headArcIt = activeJumps.find(snakeMeshNode.get());
            const bool headWasArcing = headArcIt != activeJumps.end();
            std::shared_ptr<Physic::Jump::JumpTrajectory> headTrajectory;
            if (headWasArcing) {
                headTrajectory = headArcIt->second.trajectory;
            }
            std::vector<SnakeMeshNode3D *> bodyArcingBefore;
            bodyArcingBefore.reserve(children.size());
            for (const auto &child : children) {
                const auto tile = dynamic_pointer_cast<SnakeMeshNode3D>(child);
                if (!tile) continue;
                if (activeJumps.contains(tile.get())) {
                    bodyArcingBefore.push_back(tile.get());
                }
            }

            claimBodyJumps();
            advanceAllJumps(moveInterval);

            // Head's arc just landed in a hole this tick: enter chain mode.
            // We use moveSpeed (NOT the arc's exit velocity) as the drape rate
            // so the chain stays rigid: head's depth grows at exactly the rate
            // body tiles advance toward the kink, giving 1-cell spacing across
            // drape and (since gravity then accelerates the whole chain
            // uniformly) free fall too.
            if (!chainActive && headWasArcing && headTrajectory &&
                !activeJumps.contains(snakeMeshNode.get()) &&
                voidPredicate &&
                (snakeMeshNode->x - 16) % CUBE_SIZE == 0 &&
                (snakeMeshNode->y - 16) % CUBE_SIZE == 0 &&
                voidPredicate(snakeMeshNode->x, snakeMeshNode->y)) {
                enterChainMode(static_cast<float>(UNIT_MOVE / moveInterval));
            }
            // Body tiles whose arc finished this tick land exactly at the
            // head's landing point (= kink). Join them to the chain so they
            // start dropping with the rest of the snake.
            if (chainActive) {
                for (auto *tile : bodyArcingBefore) {
                    if (!activeJumps.contains(tile) &&
                        tile->x == chainKinkVirtualX && tile->y == chainKinkVirtualY) {
                        joinChain(tile);
                    }
                }
            }
            // Drape ends the moment every tile is in the chain. From here on
            // gravity accelerates the whole chain together.
            if (chainActive && !chainAccelerating && allTilesInChain()) {
                chainAccelerating = true;
            }

            // check only, when snake is moving and not falling
            if (!chainActive &&
                snakeMeshNode->getDirection() > SnakeMeshNode3D::STOP && snakeMeshNode->getDirection() < SnakeMeshNode3D::CRASH) {
                const auto isRealSnakeCollision = [this](const shared_ptr<MeshNode3D> &body) {
                    const auto snakeBody = dynamic_pointer_cast<SnakeMeshNode3D>(body);
                    if (!snakeBody) {
                        return true;
                    }
                    return snakeMeshNode->x == snakeBody->x && snakeMeshNode->y == snakeBody->y;
                };

                const auto isFloor = [](const shared_ptr<MeshNode3D> &body) {
                    return body->getName().rfind("Floor ", 0) == 0;
                };

                if (isChangeDirectionAllowed()) {
                    for (const auto &shapeNode: snakeMeshNode->getCollisionShapes()) {
                        for (const auto &body : shapeNode->getCollidingBodies()) {
                            if (isDebug) {
                                cout << "Had narazil do objektu: " << body->getName() << endl;
                            }
                            if (body->getName() == "coin") {
                                if (eatenUpCallback) {
                                    eatenUpCallback();
                                }
                                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::STOP) { // level completed
                                    changeCallback = nullptr;
                                }
                                return;
                            }
                            // Floor is a pass-through support surface, never a wall.
                            if (isFloor(body)) {
                                continue;
                            }
                            if (!isRealSnakeCollision(body)) {
                                continue;
                            }
                            if (!crashLock && crashCallback) {
                                crashLock = true;
                                crashCallback();
                            }
                            return;
                        }
                    }
                } else { // check death
                    for (const auto &shapeNode: snakeMeshNode->getCollisionShapes()) {
                        for (const auto &body : shapeNode->getCollidingBodies()) {
                            if (body->getName() == "coin") {
                                continue;
                            }
                            if (isFloor(body)) {
                                continue;
                            }
                            if (!isRealSnakeCollision(body)) {
                                continue;
                            }
                            if (!crashLock && crashCallback) {
                                if (isDebug) {
                                    cout << "Had narazil do objektu: " << body->getName() << endl;
                                }
                                crashLock = true;
                                crashCallback();
                                return;
                            }
                        }
                    }
                }
            }
        }

    }

    bool SnakeMoveHandler::isChangeDirectionAllowed() const {
        const int x = snakeMeshNode->x - 16;
        const int y = snakeMeshNode->y - 16;

        return x % CUBE_SIZE == 0 && y % CUBE_SIZE == 0;
    }

    bool SnakeMoveHandler::isNewDirectionCorrect(const unsigned int direction) const {
        if (snakeMeshNode->getDirection() == SnakeMeshNode3D::PAUSE) {
            return false;
        }

        // At the very start (direction NONE) the body is laid out behind the
        // head along the OPPOSITE of `initialBodyDirection` (e.g. body trails
        // to the LEFT when initialBodyDirection==RIGHT). Pressing the reverse
        // would walk the head straight into its own body, so disallow it.
        if (snakeMeshNode->getDirection() == SnakeMeshNode3D::NONE) {
            SnakeMeshNode3D::eDIRECTION requested = SnakeMeshNode3D::NONE;
            switch (direction) {
                case GLFW_KEY_J: requested = SnakeMeshNode3D::LEFT;  break;
                case GLFW_KEY_L: requested = SnakeMeshNode3D::RIGHT; break;
                case GLFW_KEY_I: requested = SnakeMeshNode3D::UP;    break;
                case GLFW_KEY_K: requested = SnakeMeshNode3D::DOWN;  break;
                default: break;
            }
            SnakeMeshNode3D::eDIRECTION reverseOfBody = SnakeMeshNode3D::NONE;
            switch (initialBodyDirection) {
                case SnakeMeshNode3D::RIGHT: reverseOfBody = SnakeMeshNode3D::LEFT;  break;
                case SnakeMeshNode3D::LEFT:  reverseOfBody = SnakeMeshNode3D::RIGHT; break;
                case SnakeMeshNode3D::UP:    reverseOfBody = SnakeMeshNode3D::DOWN;  break;
                case SnakeMeshNode3D::DOWN:  reverseOfBody = SnakeMeshNode3D::UP;    break;
                default: break;
            }
            if (requested != SnakeMeshNode3D::NONE && requested == reverseOfBody) {
                return false;
            }
        }

        switch (direction) {
            case GLFW_KEY_L:
                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::LEFT || snakeMeshNode->getDirection() ==
                    SnakeMeshNode3D::RIGHT) {
                    return false;
                }
                return true;
            case GLFW_KEY_I:
                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::DOWN || snakeMeshNode->getDirection() ==
                    SnakeMeshNode3D::UP) {
                    return false;
                }
                return true;
            case GLFW_KEY_J:
                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::RIGHT ||
                    snakeMeshNode->getDirection() == SnakeMeshNode3D::LEFT ||
                    snakeMeshNode->getDirection() == SnakeMeshNode3D::STOP ||
                    snakeMeshNode->getDirection() == SnakeMeshNode3D::CRASH) {
                    return false;
                }
                return true;
            case GLFW_KEY_K:
                if (snakeMeshNode->getDirection() == SnakeMeshNode3D::UP || snakeMeshNode->getDirection() ==
                    SnakeMeshNode3D::DOWN) {
                    return false;
                }
                return true;
            default:
                return false;
        }
    }

    void SnakeMoveHandler::addStartMoveCallback(const function<void()> &startMoveCallback) {
        startMoveCallbacks.push_back(startMoveCallback);
    }

    void SnakeMoveHandler::setStopMoveCallback(const function<void(bool stop)> &stopMoveCallback) {
        SnakeMoveHandler::stopMoveCallback = stopMoveCallback;
    }

    void SnakeMoveHandler::setCrashCallback(const function<void()> &crashCallback) {
        SnakeMoveHandler::crashCallback = crashCallback;
    }

    void SnakeMoveHandler::setEatenUpCallback(const function<void()> &eatenUpCallback) {
        SnakeMoveHandler::eatenUpCallback = eatenUpCallback;
    }

    void SnakeMoveHandler::setEnabled(const bool enabled) {
        SnakeMoveHandler::enabled = enabled;
        lastTime = glfwGetTime();
        moveAccumulator = 0.0;
    }

    void SnakeMoveHandler::setStopped(const bool stopped) {
        if (changeCallback) {
            return;
        }
        if (stop == stopped) {
            return;
        }
        stop = stopped;
        stopMoveCallback(stop);
    }

    void SnakeMoveHandler::setInitialBodyDirection(const SnakeMeshNode3D::eDIRECTION direction) {
        initialBodyDirection = direction;
    }

    void SnakeMoveHandler::resetState() {
        changeCallback = nullptr;
        crashLock = false;
        stop = false;
        eatenUpCallbackCalled = false;
        lastTime = glfwGetTime();
        moveAccumulator = 0.0;
        clearAllJumps();
    }

    template<typename Iter>
    SnakeMeshNode3D::eDIRECTION SnakeMoveHandler::findDirection(Iter iter) const {
        const auto &children = snakeMeshNode->getChildren();
        const shared_ptr<SnakeMeshNode3D> tile = iter == children.begin()
                                                     ? snakeMeshNode
                                                     : dynamic_pointer_cast<SnakeMeshNode3D>(*(iter - 1));

        // Spawn-spacing gate: freshly grown tiles share their predecessor's
        // cell on the eat tick (`addTile` copies pos + virtual x/y). Without a
        // gap check, the moment the predecessor moves a single sub-cell step
        // the new tile picks up the chase via findDirection, and the entire
        // freshly grown tail ends up packed at UNIT_MOVE-spacing instead of
        // the 1-cell spacing the rest of the body has. By refusing to assign
        // a direction until the predecessor is a full cell away we let the
        // predecessor open a CUBE_SIZE gap first; from then on both tiles
        // advance in lockstep and the gap stays at exactly 1 cell - matching
        // the original snake's segment spacing. Established tiles are exactly
        // CUBE_SIZE apart so the >= check passes for them.
        const int rawDx = tile->x - iter->get()->x;
        const int rawDy = tile->y - iter->get()->y;
        const int dx = rawDx < 0 ? -rawDx : rawDx;
        const int dy = rawDy < 0 ? -rawDy : rawDy;
        if (dx + dy < CUBE_SIZE) {
            return SnakeMeshNode3D::NONE;
        }

        if (tile->getPosition().x > iter->get()->getPosition().x) {
            return SnakeMeshNode3D::RIGHT;
        }
        if (tile->getPosition().x < iter->get()->getPosition().x) {
            return SnakeMeshNode3D::LEFT;
        }
        if (tile->getPosition().y < iter->get()->getPosition().y) {
            return SnakeMeshNode3D::DOWN;
        }
        if (tile->getPosition().y > iter->get()->getPosition().y) {
            return SnakeMeshNode3D::UP;
        }

        return SnakeMeshNode3D::NONE;
    }
} // Handler
