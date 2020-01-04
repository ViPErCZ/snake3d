#include <GLES2/gl2.h>
#include "stdafx.h"
#include "SnakeContainer.h"
#include "HeadSprite.h"

SnakeContainer::SnakeContainer(void)
= default;

SnakeContainer::~SnakeContainer(void)
= default;

void SnakeContainer::Init() {
    Vector3f pos = {0, 0, 0};
    auto *snakeTile = new sSNAKE_TILE;
    snakeTile->tile = new HeadSprite();
    snakeTile->tile->setTexture((char *) "images/head.bmp");
    pos.x = 368;
    pos.y = 368;
    pos.z = 15;
    snakeTile->tile->setPos(pos);
    Vector3f zoom = {0, 0, 0};
    zoom.x = 15.0;
    zoom.y = 15.0;
    zoom.z = 15.0;
    snakeTile->tile->setZoom(zoom);
    snakeTile->direction = STOP;
    snakeTile->alpha = 1.0;

    snake.push_back(snakeTile);

    tileTexture = Sprite::loadTexture((char *) "images/tile.bmp");
    AddTile(LEFT);
    AddTile(LEFT);
    AddTile(LEFT);
}

void SnakeContainer::AddTile(eDIRECTION aDirection) {
    if (!snake.empty()) {
        auto *snakeTile = new sSNAKE_TILE;
        snakeTile->tile = new CubeSprite();
        Vector3f pos = {0, 0, 15};
        Vector3f zoom = {0, 0, 0};

        auto Iter = snake.end() - 1;

        if ((*(snake.begin()))->direction != STOP) { // po pridani tela se rozhoduje, zda se ma novy kus rozbehnout
            pos = (*Iter)->tile->getPos();
            if (((int) pos.x - 16) % CUBE_SIZE != 0 || ((int) pos.y - 16) % CUBE_SIZE != 0) {
                if ((*Iter)->direction == RIGHT) {
                    pos.x -= UNIT_MOVE;
                } else if ((*Iter)->direction == LEFT) {
                    pos.x += UNIT_MOVE;
                } else if ((*Iter)->direction == UP) {
                    pos.y -= UNIT_MOVE;
                } else {
                    pos.y += UNIT_MOVE;
                }
            }
        } else {
            switch (aDirection) {
                case LEFT:
                    if ((*Iter)->tile->getPos().x - 32 >= 16) {
                        pos.x = (*Iter)->tile->getPos().x - 32;
                        pos.y = (*Iter)->tile->getPos().y;
                    } else {
                        delete snakeTile;
                        return;
                    }
                    break;
                case RIGHT:
                    if ((*Iter)->tile->getPos().x + 32 <= 752) {
                        pos.x = (*Iter)->tile->getPos().x + 32;
                        pos.y = (*Iter)->tile->getPos().y;
                    } else {
                        delete snakeTile;
                        return;
                    }
                    break;
                case UP:
                    if ((*Iter)->tile->getPos().y - 32 >= 16) {
                        pos.x = (*Iter)->tile->getPos().x;
                        pos.y = (*Iter)->tile->getPos().y - 32;
                    } else {
                        delete snakeTile;
                        return;
                    }
                    break;
                case DOWN:
                    if ((*Iter)->tile->getPos().y + 32 <= 752) {
                        pos.x = (*Iter)->tile->getPos().x;
                        pos.y = (*Iter)->tile->getPos().y + 32;
                    } else {
                        delete snakeTile;
                        return;
                    }
                    break;
                case STOP:
                case CRASH:
                    break;
            }
        }

        snakeTile->tile->addTexture(tileTexture);
        snakeTile->tile->setPos(pos);
        zoom.x = 15.0;
        zoom.y = 15.0;
        zoom.z = 15.0;
        snakeTile->tile->setZoom(zoom);
        snakeTile->direction = STOP;
        snakeTile->alpha = 1.0;
        //snakeTile->tile->setVisibility(visibility);
        snake.push_back(snakeTile);
    }
}

void SnakeContainer::Release() {
    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        (*Iter)->tile->Release();
        delete (*Iter)->tile;
        delete (*Iter);
    }

    snake.clear();
}

void SnakeContainer::Render() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float prevAlpha = 0;

    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        //glColorMask(GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
        if ((*snake.begin())->direction == CRASH && (*Iter)->tile->getVisibility()) {
            glColor4f(0.1, 0.2f, 0.1, (*Iter)->alpha);
            if (Iter == snake.begin() || prevAlpha < 0.5) {
                if ((*Iter)->alpha <= 0) {
                    (*Iter)->tile->setVisibility(false);
                } else {
                    (*Iter)->alpha -= 0.008;
                }
            }
            prevAlpha = (*Iter)->alpha;
        }
        if ((*Iter)->tile->getVisibility()) {
            (*Iter)->tile->Render();
        }
    }

    glEnable(GL_LIGHTING);
}

void SnakeContainer::Move(eDIRECTION aDirection) {
    Vector3f pos = {0, 0, 15};
    Vector3f prevPos = {0, 0, 0};
    eDIRECTION prevDirection = STOP;
    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        if (Iter != snake.begin()) {
            if (aDirection == STOP || (*snake.begin())->direction == CRASH) {
                pos = (*Iter)->tile->getPos();
            } else {
                if ((*Iter)->direction == STOP) {
                    if (((*Iter)->tile->getPos().x + 32 == prevPos.x) ||
                        ((*Iter)->tile->getPos().x - 32 == prevPos.x) ||
                        ((*Iter)->tile->getPos().y + 32 == prevPos.y) ||
                        ((*Iter)->tile->getPos().y - 32 == prevPos.y)
                            ) {
                        (*Iter)->direction = aDirection;
                        //pos = (*Iter)->tile->getPos();
                    } else {
                        pos = (*Iter)->tile->getPos();
                    }
                }

                if ((*Iter)->direction != STOP) {
                    switch (prevDirection) {
                        case LEFT:
                            if ((*Iter)->tile->getPos().y != prevPos.y) {
                                if ((*Iter)->tile->getPos().y < prevPos.y) {
                                    pos.y = (*Iter)->tile->getPos().y + UNIT_MOVE;
                                    (*Iter)->direction = UP;
                                } else {
                                    pos.y = (*Iter)->tile->getPos().y - UNIT_MOVE;
                                    (*Iter)->direction = DOWN;
                                }
                                pos.x = (*Iter)->tile->getPos().x;
                            } else {
                                pos.x = (*Iter)->tile->getPos().x - UNIT_MOVE;
                                pos.y = (*Iter)->tile->getPos().y;
                                (*Iter)->direction = LEFT;
                            }
                            break;
                        case RIGHT:
                            if ((*Iter)->tile->getPos().y != prevPos.y) {
                                if ((*Iter)->tile->getPos().y < prevPos.y) {
                                    pos.y = (*Iter)->tile->getPos().y + UNIT_MOVE;
                                    (*Iter)->direction = UP;
                                } else {
                                    pos.y = (*Iter)->tile->getPos().y - UNIT_MOVE;
                                    (*Iter)->direction = DOWN;
                                }
                                pos.x = (*Iter)->tile->getPos().x;
                            } else {
                                pos.x = (*Iter)->tile->getPos().x + UNIT_MOVE;
                                pos.y = (*Iter)->tile->getPos().y;
                                (*Iter)->direction = RIGHT;
                            }
                            break;
                        case UP:
                            if ((*Iter)->tile->getPos().x != prevPos.x) {
                                if ((*Iter)->tile->getPos().x < prevPos.x) {
                                    pos.x = (*Iter)->tile->getPos().x + UNIT_MOVE;
                                    (*Iter)->direction = RIGHT;
                                } else {
                                    pos.x = (*Iter)->tile->getPos().x - UNIT_MOVE;
                                    (*Iter)->direction = LEFT;
                                }
                                pos.y = (*Iter)->tile->getPos().y;
                            } else {
                                pos.x = (*Iter)->tile->getPos().x;
                                pos.y = (*Iter)->tile->getPos().y + UNIT_MOVE;
                                (*Iter)->direction = UP;
                            }
                            break;
                        case DOWN:
                            if ((*Iter)->tile->getPos().x != prevPos.x) {
                                if ((*Iter)->tile->getPos().x < prevPos.x) {
                                    pos.x = (*Iter)->tile->getPos().x + UNIT_MOVE;
                                    (*Iter)->direction = RIGHT;
                                } else {
                                    pos.x = (*Iter)->tile->getPos().x - UNIT_MOVE;
                                    (*Iter)->direction = LEFT;
                                }
                                pos.y = (*Iter)->tile->getPos().y;
                            } else {
                                pos.x = (*Iter)->tile->getPos().x;
                                pos.y = (*Iter)->tile->getPos().y - UNIT_MOVE;
                                (*Iter)->direction = DOWN;
                            }
                            break;
                    }
                }
            }
            //pos = prevPos; // tady ulozim predchozi polohu a predpokladam, ze je o celou kosticku, a na tuto polohu nastavim aktualni, protoze predchozi se o celou delku posunula
            prevPos = (*Iter)->tile->getPos();
            prevDirection = (*Iter)->direction;
        } else {
            prevPos = (*Iter)->tile->getPos();
            switch (aDirection) {
                case STOP:
                    pos.x = (*Iter)->tile->getPos().x;
                    pos.y = (*Iter)->tile->getPos().y;
                    (*Iter)->direction = STOP;
                    break;
                case CRASH:
                    pos.x = (*Iter)->tile->getPos().x;
                    pos.y = (*Iter)->tile->getPos().y;
                    (*Iter)->direction = CRASH;
                    break;
                case LEFT:
                    pos.x = (*Iter)->tile->getPos().x;
                    if (pos.x > 16) {
                        pos.x = (*Iter)->tile->getPos().x - UNIT_MOVE;
                        (*Iter)->direction = LEFT;
                    } else
                        (*Iter)->direction = CRASH;
                    pos.y = (*Iter)->tile->getPos().y;
                    break;
                case RIGHT:
                    pos.x = (*Iter)->tile->getPos().x;
                    if (pos.x < 1520) {
                        pos.x = (*Iter)->tile->getPos().x + UNIT_MOVE;
                        (*Iter)->direction = RIGHT;
                    } else
                        (*Iter)->direction = CRASH;
                    pos.y = (*Iter)->tile->getPos().y;
                    break;
                case UP:
                    pos.x = (*Iter)->tile->getPos().x;
                    pos.y = (*Iter)->tile->getPos().y;
                    if (pos.y < 752) {
                        pos.y = (*Iter)->tile->getPos().y + UNIT_MOVE;
                        (*Iter)->direction = UP;
                    } else
                        (*Iter)->direction = CRASH;
                    break;
                case DOWN:
                    pos.x = (*Iter)->tile->getPos().x;
                    pos.y = (*Iter)->tile->getPos().y;
                    if (pos.y > 16) {
                        pos.y = (*Iter)->tile->getPos().y - UNIT_MOVE;
                        (*Iter)->direction = DOWN;
                    } else
                        (*Iter)->direction = CRASH;
                    break;
            }
            Vector2f testCrash = {pos.x, pos.y};
            if (isSnakeInThisPos(Iter, testCrash)) {
                (*Iter)->direction = CRASH;
                return;
            }
            prevDirection = (*Iter)->direction;
        }
        if ((*Iter)->direction != STOP) {
            (*Iter)->tile->setPos(pos);
        }
    }
}

Vector2f SnakeContainer::getHeadPos() {
    Vector2f pos = {(*snake.begin())->tile->getPos().x, (*snake.begin())->tile->getPos().y};

    return pos;
}

bool SnakeContainer::isSnakeInThisPos(vector<sSNAKE_TILE*>::const_iterator myself, Vector2f pos) {
    //Staci kontrolovat jestli hlave nestoji nic v jejim smeru
    pos = {(*snake.begin())->tile->getPos().x, (*snake.begin())->tile->getPos().y};

    for (auto Iter = snake.begin() + 2; Iter < snake.end(); Iter++) {
        if (((*Iter)->tile->getPos().x > pos.x && (*Iter)->tile->getPos().x < pos.x + 32
             && (*Iter)->tile->getPos().y < pos.y && (*Iter)->tile->getPos().y > pos.y - 32) || (
                    (*Iter)->tile->getPos().x + 32 > pos.x && (*Iter)->tile->getPos().x + 32 < pos.x + 32
                    && (*Iter)->tile->getPos().y - 32 < pos.y && (*Iter)->tile->getPos().y - 32 > pos.y - 32
            )) {
            return true;
        }
    }

    return false;
}

Vector2f SnakeContainer::getFreeArray() {
    Vector2f pos = {-1, -1};

    for (int x = 0; x < 23; x++) {
        for (int y = 0; y < 23; y++) {
            bool newPos = true;
            for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
                if ((*Iter)->tile->getPos().x == ((x + 1) * 32) - 16 &&
                    (*Iter)->tile->getPos().y == ((y + 1) * 32) - 16) {
                    newPos = false;
                }
            }
            if (newPos) {
                pos.x = ((x + 1) * 32) - 16;
                pos.y = ((y + 1) * 32) - 16;

                return pos;
            }
        }
    }

    return pos;
}

bool SnakeContainer::isSnakeCrash() {
    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        if ((*Iter)->direction == CRASH) {
            return true;
        }
    }

    return false;
}

bool SnakeContainer::isChangeDirectionAllowed() {
    int x = (int) (*snake.begin())->tile->getPos().x - 16;
    int y = (int) (*snake.begin())->tile->getPos().y - 16;

    return x % CUBE_SIZE == 0 && y % CUBE_SIZE == 0;
}

int SnakeContainer::getSize() {
    return snake.size();
}

bool SnakeContainer::isSnakeInThisPos(Vector2f pos) {
    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        if (((*Iter)->tile->getPos().x >= pos.x && (*Iter)->tile->getPos().x <= pos.x + 32
             && (*Iter)->tile->getPos().y <= pos.y && (*Iter)->tile->getPos().y >= pos.y - 32) || (
                    (*Iter)->tile->getPos().x + 32 >= pos.x && (*Iter)->tile->getPos().x + 32 <= pos.x + 32
                    && (*Iter)->tile->getPos().y - 32 <= pos.y && (*Iter)->tile->getPos().y - 32 >= pos.y - 32
            )) {
            return true;
        }
    }

    return false;
}

void SnakeContainer::reset() {
    for (auto Iter = snake.end() - 1; Iter >= snake.begin(); Iter--) {
        if (snake.size() == 4) {
            break;
        }
        delete (*Iter)->tile;
        delete (*Iter);

        snake.erase(Iter);
    }

    Vector3f pos = {0, 0, 0};
    auto snakeTile = (*snake.begin());
    pos.x = 368;
    pos.y = 368;
    pos.z = 15;
    snakeTile->tile->setPos(pos);
    snakeTile->direction = STOP;
    snakeTile->alpha = 1.0;
    snakeTile->tile->setVisibility(true);

    for (auto Iter = snake.begin() + 1; Iter < snake.end(); Iter++) {
        pos.x -= 32;
        auto tile = (*Iter);
        tile->tile->setPos(pos);
        tile->direction = STOP;
        tile->alpha = 1.0;
        tile->tile->setVisibility(true);
    }
}

bool SnakeContainer::isSnakeVisible() {
    for (auto Iter = snake.begin(); Iter < snake.end(); Iter++) {
        if ((*Iter)->tile->getVisibility()) {
            return true;
        }
    }

    return false;
}

eDIRECTION SnakeContainer::getHeadDirection() {
    return (*snake.begin())->direction;
}
