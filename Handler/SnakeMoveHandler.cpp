#include "SnakeMoveHandler.h"

namespace Handler {
    SnakeMoveHandler::SnakeMoveHandler(Snake *snake, AnimationModel* animHead) : snake(snake), animHead(animHead), eatenUpCallbackCalled(false) {
        snakeHead = (*snake->getItems().begin());
        stop = false;
    }

    SnakeMoveHandler::~SnakeMoveHandler() = default;

    void SnakeMoveHandler::onEventHandler(unsigned int key) {
        switch (key) {
            case GLFW_KEY_L:
            case GLFW_KEY_I:
            case GLFW_KEY_J:
            case GLFW_KEY_K:
                if (!stop) {
                    ChangeMove(key);
                }
                break;
            case GLFW_KEY_SPACE:
                StopMove();
                break;
            default:
                break;
        }
    }

    void SnakeMoveHandler::StopMove() {
        if (!changeCallback) {
            stop = !stop;
            animHead->setGlobalPause(stop);
        }
    }

    void SnakeMoveHandler::ChangeMove(unsigned int direction) {

        if (changeCallback ||
            !isNewDirectionCorrect(snakeHead, direction)) {
            return;
        }

        createChangeCallback(direction);
    }

    void SnakeMoveHandler::createChangeCallback(unsigned int direction) {
        changeCallback = [this, direction](sSNAKE_TILE *head) {
            if (isChangeDirectionAllowed(head)) {

                int x = (int) head->tile->getPosition().x - 1;
                int y = (int) head->tile->getPosition().y - 1;
                if (x % CUBE_SIZE == 0 && y % CUBE_SIZE == 0) {
                    cout << "Zmena smeru povolena: " << head->tile->getPosition().x << ", " << head->tile->getPosition().y << endl;
                }

                if (head->direction == STOP && startMoveCallback) { // start hry
                    startMoveCallback();
                }

                switch (direction) {
                    case GLFW_KEY_J: // left
                        head->direction = ItemsDto::LEFT;
                        break;
                    case GLFW_KEY_L: // right
                        head->direction = ItemsDto::RIGHT;
                        break;
                    case GLFW_KEY_I:
                        head->direction = UP;
                        break;
                    case GLFW_KEY_K:
                        head->direction = DOWN;
                        break;
                    default:
                        break;
                }

                for (auto Iter = snake->getItems().begin() + 1; Iter < snake->getItems().end(); Iter++) {
                    // prvni rozbehnuti tela je vzdy vpravo, protoze na startu je hlava vpravo od tela
                    if ((*Iter)->direction == NONE) {
                        (*Iter)->direction = ItemsDto::RIGHT;
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

        double now = glfwGetTime();

        if (now - next_time >= 0.005) {
            if (changeCallback) {
                if (changeCallback(snakeHead)) {
                    changeCallback = nullptr;
                }
            }

            if (snakeHead->direction == CRASH) { // pokud je hlava mrtva, resetujeme hada
                snake->reset();
                return;
            }

            if (snakeHead->direction == NONE || snakeHead->direction == STOP) {
                return;
            }

            bool allowed = isChangeDirectionAllowed(snakeHead);

            for (auto Iter = snake->getItems().end() - 1; Iter >= snake->getItems().begin(); Iter--) {
                eDIRECTION direction = (*Iter)->direction;

                if (snake->getItems().begin() != Iter && allowed) {
                    auto PrevIter = Iter - 1;
                    direction = findDirection((*PrevIter), (*Iter));
                    (*Iter)->direction = direction;
                }

                glm::vec3 pos = (*Iter)->tile->getPosition();
                switch (direction) {
                    case ItemsDto::LEFT:
                        pos.x -= UNIT_MOVE;
                        (*Iter)->tile->setVirtualX((*Iter)->tile->getVirtualX() - VIRTUAL_MOVE);
                        break;
                    case ItemsDto::RIGHT:
                        pos.x += UNIT_MOVE;
                        (*Iter)->tile->setVirtualX((*Iter)->tile->getVirtualX() + VIRTUAL_MOVE);
                        break;
                    case UP:
                        pos.y += UNIT_MOVE;
                        (*Iter)->tile->setVirtualY((*Iter)->tile->getVirtualY() + VIRTUAL_MOVE);
                        break;
                    case DOWN:
                        pos.y -= UNIT_MOVE;
                        (*Iter)->tile->setVirtualY((*Iter)->tile->getVirtualY() - VIRTUAL_MOVE);
                        break;
                    case CRASH:
                    case PAUSE:
                    case STOP:
                    case NONE:
                        break;
                }
                (*Iter)->tile->setPosition(pos);

            }
            next_time = now;

            // detekujeme jen kdyz je predmet na kterem detekujeme v pohybu
            if (snakeHead->direction > STOP && snakeHead->direction < CRASH) {
                // pokud je hlava a pohnula se, tak checkneme zda je komplet v hraci kosticce
                // pokud ano, tak checkneme kolizi s jidlem
                bool allowed = isChangeDirectionAllowed(snakeHead);
                if (allowed && collisionDetector->detectWithStaticItem(snakeHead->tile)) {
                    cout << "Head position(eaten): " << snakeHead->tile->getPosition().x << ", " << snakeHead->tile->getPosition().y << endl;
                    eatenUpCallback();
                    if (snakeHead->direction == STOP) { // doslo k postupu do dalsiho level
                        changeCallback = nullptr;
                    }
                }

                if (collisionDetector->perimeterDetect(snakeHead->tile)
                    || collisionDetector->barrierCollision(snakeHead->tile)
                    || Physic::CollisionDetector::intoHimSelf(snake)
                ) {
                    if (crashCallback) {
                        crashCallback(); // doslo k narazu
                    }
                    changeCallback = nullptr;
                    for (auto Iter = snake->getItems().begin(); Iter < snake->getItems().end(); Iter++) {
                        (*Iter)->direction = CRASH;
                        (*Iter)->prevPauseDirection = NONE;
                    }
                }
            }
        }
    }

    bool SnakeMoveHandler::isChangeDirectionAllowed(sSNAKE_TILE *snake) {
        int x = (int) snake->tile->getVirtualX() - 16;
        int y = (int) snake->tile->getVirtualY() - 16;

        return x % CUBE_SIZE == 0 && y % CUBE_SIZE == 0;
    }

    bool SnakeMoveHandler::isNewDirectionCorrect(sSNAKE_TILE *headTile, unsigned int direction) {

        if (headTile->direction == PAUSE) {
            return false;
        }

        switch (direction) {
            case GLFW_KEY_L:
                if (headTile->direction == ItemsDto::LEFT || headTile->direction == ItemsDto::RIGHT) {
                    return false;
                }
                return true;
            case GLFW_KEY_I:
                if (headTile->direction == DOWN || headTile->direction == UP) {
                    return false;
                }
                return true;
            case GLFW_KEY_J:
                if (headTile->direction == ItemsDto::RIGHT || headTile->direction == ItemsDto::LEFT || headTile->direction == STOP || headTile->direction == CRASH) {
                    return false;
                }
                return true;
            case GLFW_KEY_K:
                if (headTile->direction == UP || headTile->direction == DOWN) {
                    return false;
                }
                return true;
            default:
                return false;
        }
    }

    void SnakeMoveHandler::setCollisionDetector(CollisionDetector *collisionDetector) {
        SnakeMoveHandler::collisionDetector = collisionDetector;
    }

    void SnakeMoveHandler::setStartMoveCallback(const function<void(void)> &startMoveCallback) {
        SnakeMoveHandler::startMoveCallback = startMoveCallback;
    }

    void SnakeMoveHandler::setCrashCallback(const function<void()> &crashCallback) {
        SnakeMoveHandler::crashCallback = crashCallback;
    }

    void SnakeMoveHandler::setEatenUpCallback(const function<void()> &eatenUpCallback) {
        SnakeMoveHandler::eatenUpCallback = eatenUpCallback;
    }

    eDIRECTION SnakeMoveHandler::findDirection(sSNAKE_TILE* snakeTile, sSNAKE_TILE* mySelf) {
        // najdi kosticku co je hned vedle
        if (snakeTile->tile->getPosition().x > mySelf->tile->getPosition().x) {
            return ItemsDto::RIGHT;
        } else if (snakeTile->tile->getPosition().x < mySelf->tile->getPosition().x) {
            return ItemsDto::LEFT;
        } else if (snakeTile->tile->getPosition().y < mySelf->tile->getPosition().y) {
            return DOWN;
        } else if (snakeTile->tile->getPosition().y > mySelf->tile->getPosition().y) {
            return UP;
        }

        return NONE;
    }

} // Handler