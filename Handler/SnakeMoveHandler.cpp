#include "SnakeMoveHandler.h"

namespace Handler {
    SnakeMoveHandler::SnakeMoveHandler(const shared_ptr<SnakeMeshNode3D> &snake_mesh_node)
        : snakeMeshNode(snake_mesh_node), eatenUpCallbackCalled(false) {
        stop = false;
        lastTime = glfwGetTime();
        moveAccumulator = 0.0;
        constexpr double stepsPerTile = static_cast<double>(CUBE_SIZE) / static_cast<double>(VIRTUAL_MOVE);
        constexpr double tilesPerSecond = 3.5;
        moveInterval = 1.0 / (tilesPerSecond * stepsPerTile);
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
        if (jumpRequested || isHeadAirborne()) {
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
        return activeJumps.find(snakeMeshNode.get()) != activeJumps.end();
    }

    void SnakeMoveHandler::startHeadJump() {
        glm::vec3 directionVector(0.0f);
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
        auto trajectory = std::make_shared<Physic::Jump::JumpTrajectory>(headPos, endPos, duration, peakHeight);

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
            if (activeJumps.find(bodyTile.get()) != activeJumps.end()) {
                continue; // already arcing
            }
            // First pending event matching this tile's exact cell wins (FIFO).
            for (auto it = pendingTakeoffs.begin(); it != pendingTakeoffs.end(); ++it) {
                if (it->tilesRemaining <= 0) {
                    continue;
                }
                if (bodyTile->x == it->virtualX && bodyTile->y == it->virtualY) {
                    activeJumps[bodyTile.get()] = TileJumpState{it->trajectory, 0.0, bodyTile->getPosition().z};
                    it->tilesRemaining -= 1;
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
                it = activeJumps.erase(it);
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
        }

        if (snakeMeshNode->isCrashing()) {
            moveAccumulator = 0;
            lastTime = glfwGetTime();
            clearAllJumps();
            return;
        }

        if (stop) {
            return;
        }

        moveAccumulator += deltaTime;

        while (moveAccumulator >= moveInterval) {
            moveAccumulator -= moveInterval;

            if (changeCallback) {
                if (changeCallback(snakeMeshNode)) {
                    changeCallback = nullptr;
                }
            }

            if (snakeMeshNode->getDirection() == SnakeMeshNode3D::NONE || snakeMeshNode->getDirection() ==
                SnakeMeshNode3D::STOP) {
                moveAccumulator = 0;
                return;
            }

            const bool allowed = isChangeDirectionAllowed();
            const auto &children = snakeMeshNode->getChildren();

            for (auto Iter = children.rbegin(); Iter != children.rend(); ++Iter) {
                const auto &tile = dynamic_pointer_cast<SnakeMeshNode3D>(*Iter);

                if (allowed) {
                    tile->setDirection(findDirection(std::next(Iter).base()));
                }

                moveTile(tile);
            }

            if (allowed && jumpRequested && !isHeadAirborne()) {
                startHeadJump();
            }

            moveTile(snakeMeshNode);

            claimBodyJumps();
            advanceAllJumps(moveInterval);

            // check only, when snake is moving
            if (snakeMeshNode->getDirection() > SnakeMeshNode3D::STOP && snakeMeshNode->getDirection() < SnakeMeshNode3D::CRASH) {
                const auto isRealSnakeCollision = [this](const shared_ptr<MeshNode3D> &body) {
                    const auto snakeBody = dynamic_pointer_cast<SnakeMeshNode3D>(body);
                    if (!snakeBody) {
                        return true;
                    }
                    return snakeMeshNode->x == snakeBody->x && snakeMeshNode->y == snakeBody->y;
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
