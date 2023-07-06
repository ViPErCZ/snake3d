#include "Snake.h"

namespace ItemsDto {
    Snake::~Snake() {
        Snake::release();
    }

    void Snake::release() {
        for (auto Iter = tiles.begin(); Iter < tiles.end(); Iter++) {
            delete (*Iter)->tile;
            delete (*Iter);
        }

        tiles.clear();
    }

    void Snake::init() {
        auto *snakeTile = new sSNAKE_TILE;
        snakeTile->tile = new Cube();
        snakeTile->tile->setVisible(true);
        snakeTile->alpha = 1.0;

        tiles.push_back(snakeTile);
        reset();
    }

    void Snake::addTile(eDIRECTION aDirection) {
        if (!tiles.empty()) {
            auto *snakeTile = new sSNAKE_TILE;
            snakeTile->tile = new Cube();
            glm::vec3 pos = {-19, 67, -23};

            auto PrevIter = tiles.end()-1;

            if ((*(tiles.begin()))->direction != STOP) {
                pos = (*PrevIter)->tile->getPosition();
            } else {
                switch (aDirection) {
                    case LEFT:
                        if ((*PrevIter)->tile->getPosition().x - 2 >= -25) {
                            pos.x = (*PrevIter)->tile->getPosition().x - 2;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case RIGHT:
                        if ((*PrevIter)->tile->getPosition().x + 2 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x + 2;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case UP:
                        if ((*PrevIter)->tile->getPosition().y - 2 >= -25) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y - 2;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case DOWN:
                        if ((*PrevIter)->tile->getPosition().y + 2 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y + 2;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    default:
                        break;
                }
            }

            snakeTile->tile->setVirtualX((((int)(pos.x - (-23)) / 2) * 32) + 16);
            snakeTile->tile->setVirtualY((((int)(pos.y - (-23)) / 2) * 32) + 16);
            snakeTile->tile->setPosition(pos);
            snakeTile->tile->setVisible(true);
            snakeTile->prevPauseDirection = NONE;
            tiles.push_back(snakeTile);
        }
    }

    const vector<sSNAKE_TILE *> &Snake::getItems() const {
        return tiles;
    }

    Cube *Snake::getHeadTile() {
        if (!tiles.empty()) {
            return (*tiles.begin())->tile;
        }

        return nullptr;
    }

    int Snake::getMaxX() const {
        return 0;
    }

    int Snake::getMaxY() const {
        return 0;
    }

    int Snake::getMinX() const {
        return 0;
    }

    int Snake::getMinY() const {
        return 0;
    }

    void Snake::reset() {
        for (auto Iter = tiles.end() - 1; Iter != tiles.begin(); Iter--) {
            delete (*Iter)->tile;
            delete (*Iter);

            tiles.erase(Iter);
        }

        sSNAKE_TILE* snakeTile = (*tiles.begin());

        snakeTile->tile->setPosition({23, -3, -23}); // start pozice
//        snakeTile->tile->setPosition({45, -3, -23});
        snakeTile->tile->setVirtualX((((int)(23 - (-23)) / 2) * 32) + 16);
        snakeTile->tile->setVirtualY((((int)(-3 - (-23)) / 2) * 32) + 16);
        snakeTile->direction = STOP;
        snakeTile->prevPauseDirection = NONE;

        addTile(LEFT);
        addTile(LEFT);
        addTile(LEFT);
    }

} // ItemsDto