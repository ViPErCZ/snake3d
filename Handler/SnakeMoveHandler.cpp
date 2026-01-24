#include "SnakeMoveHandler.h"

namespace Handler {
    SnakeMoveHandler::SnakeMoveHandler(const shared_ptr<SnakeMeshNode3D> &snake_mesh_node)
        : snakeMeshNode(snake_mesh_node), eatenUpCallbackCalled(false) {
        stop = false;
    }

    SnakeMoveHandler::~SnakeMoveHandler() = default;

    void SnakeMoveHandler::onEventHandler(const unsigned int key, int scancode, const int action, int mods) {
        switch (key) {
            case GLFW_KEY_L:
            case GLFW_KEY_I:
            case GLFW_KEY_J:
            case GLFW_KEY_K:
                if (!stop && snakeMeshNode->isReady()) {
                    changeMove(key);
                }
                break;
            case GLFW_KEY_SPACE:
                stopMove();
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
                    // prvni rozbehnuti tela je vzdy vpravo, protoze na startu je hlava vpravo od tela
                    const auto snakeMesh = dynamic_pointer_cast<SnakeMeshNode3D>(node);
                    const auto nodeDirection = snakeMesh->getDirection();
                    if (nodeDirection == SnakeMeshNode3D::NONE) {
                        snakeMesh->setDirection(SnakeMeshNode3D::RIGHT);
                    }
                }

                return true;
            }

            return false;
        };
    }

    void SnakeMoveHandler::onDefaultHandler() {
        if (stop) {
            return;
        }

        const double now = glfwGetTime();

        if (now - next_time >= 0.00005) {
            if (changeCallback) {
                if (changeCallback(snakeMeshNode)) {
                    changeCallback = nullptr;
                }
            }

            if (snakeMeshNode->getDirection() == SnakeMeshNode3D::NONE || snakeMeshNode->getDirection() ==
                SnakeMeshNode3D::STOP) {
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

            moveTile(snakeMeshNode);

            next_time = now;

            //
            // detekujeme jen kdyz je predmet na kterem detekujeme v pohybu
            if (collisionDetector && snakeMeshNode->getDirection() > SnakeMeshNode3D::STOP && snakeMeshNode->getDirection() < SnakeMeshNode3D::CRASH) {
                 // pokud je hlava a pohnula se, tak checkneme zda je komplet v hraci kosticce
                 // pokud ano, tak checkneme kolizi s jidlem
                 const bool l_allowed = isChangeDirectionAllowed();
                 if (l_allowed && collisionDetector->detectWithStaticItem(snakeMeshNode)) {
                     cout << "Head position(eaten): " << snakeMeshNode->getPosition().x << ", " << snakeMeshNode->getPosition().y << endl;
                     if (eatenUpCallback) {
                         eatenUpCallback();
                     }
                     if (snakeMeshNode->getDirection() == SnakeMeshNode3D::STOP) { // doslo k postupu do dalsiho level
                         changeCallback = nullptr;
                     }
                 }
            //
            //     if (collisionDetector->perimeterDetect(snakeHead->tile)
            //         || collisionDetector->barrierCollision(snakeHead->tile)
            //         || CollisionDetector::intoHimSelf(snake)
            //     ) {
            //         if (crashCallback) {
            //             crashCallback(); // doslo k narazu
            //         }
            //         changeCallback = nullptr;
            //     }
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

    void SnakeMoveHandler::setCollisionDetector(shared_ptr<CollisionDetector> collisionDetector) {
        SnakeMoveHandler::collisionDetector = std::move(collisionDetector);
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
