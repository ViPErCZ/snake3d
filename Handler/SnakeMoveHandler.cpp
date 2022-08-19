#include "SnakeMoveHandler.h"

namespace Handler {
    SnakeMoveHandler::SnakeMoveHandler(Snake *snake) : snake(snake) {
        snakeHead = (*snake->getItems().begin());
        stop = false;
    }

    SnakeMoveHandler::~SnakeMoveHandler() = default;

    void SnakeMoveHandler::onEventHandler(unsigned int key) {
//        switch (key) {
//            case SDLK_l:
//            case SDLK_i:
//            case SDLK_j:
//            case SDLK_k:
//                if (!stop) {
//                    ChangeMove(key);
//                }
//                break;
//            case SDLK_SPACE:
//                StopMove();
//                break;
//            default:
//                break;
//        }
    }

    void SnakeMoveHandler::StopMove() {
        if (!changeCallback) {
            stop = !stop;
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

                if (head->direction == STOP && startMoveCallback) { // start hry
                    startMoveCallback();
                }

//                switch (direction) {
//                    case SDLK_j: // left
//                        head->direction = LEFT;
//                        break;
//                    case SDLK_l: // right
//                        head->direction = RIGHT;
//                        break;
//                    case SDLK_i:
//                        head->direction = UP;
//                        break;
//                    case SDLK_k:
//                        head->direction = DOWN;
//                        break;
//                    default:
//                        break;
//                }

                for (auto Iter = snake->getItems().begin() + 1; Iter < snake->getItems().end(); Iter++) {
                    // prvni rozbehnuti tela je vzdy vpravo, protoze na startu je hlava vpravo od tela
                    if ((*Iter)->direction == NONE) {
                        (*Iter)->direction = RIGHT;
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

        if (now - next_time >= 12) {
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
                    case LEFT:
                        pos.x -= UNIT_MOVE;
                        break;
                    case RIGHT:
                        pos.x += UNIT_MOVE;
                        break;
                    case UP:
                        pos.y += UNIT_MOVE;
                        break;
                    case DOWN:
                        pos.y -= UNIT_MOVE;
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
                    if (snakeHead->direction == STOP) { // doslo k postupu do dalsiho levelu
                        changeCallback = nullptr;
                    }
                }

                if (collisionDetector->perimeterDetect(snakeHead->tile)
                    || collisionDetector->barrierCollision(snakeHead->tile)
                    || collisionDetector->intoHimSelf(snake)
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
        int x = (int) snake->tile->getPosition().x - 16;
        int y = (int) snake->tile->getPosition().y - 16;

        return x % CUBE_SIZE == 0 && y % CUBE_SIZE == 0;
    }

    bool SnakeMoveHandler::isNewDirectionCorrect(sSNAKE_TILE *headTile, unsigned int direction) {

        if (headTile->direction == PAUSE) {
            return false;
        }

//        switch (direction) {
//            case SDLK_l:
//                if (headTile->direction == LEFT || headTile->direction == RIGHT) {
//                    return false;
//                }
//                return true;
//            case SDLK_i:
//                if (headTile->direction == DOWN || headTile->direction == UP) {
//                    return false;
//                }
//                return true;
//            case SDLK_j:
//                if (headTile->direction == RIGHT || headTile->direction == LEFT || headTile->direction == STOP || headTile->direction == CRASH) {
//                    return false;
//                }
//                return true;
//            case SDLK_k:
//                if (headTile->direction == UP || headTile->direction == DOWN) {
//                    return false;
//                }
//                return true;
//            default:
//                return false;
//        }
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
            return RIGHT;
        } else if (snakeTile->tile->getPosition().x < mySelf->tile->getPosition().x) {
            return LEFT;
        } else if (snakeTile->tile->getPosition().y < mySelf->tile->getPosition().y) {
            return DOWN;
        } else if (snakeTile->tile->getPosition().y > mySelf->tile->getPosition().y) {
            return UP;
        }

        return NONE;
    }

} // Handler