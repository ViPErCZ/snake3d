#include "Snake.h"

namespace ItemsDto {

    void Snake::init() {
        const auto snakeTile = make_shared<sSNAKE_TILE>();
        snakeTile->tile = make_shared<Cube>();
        snakeTile->tile->setVisible(true);
        snakeTile->alpha = 1.0;
        snakeTile->tile->setScale({0.041667f, 0.041667f, 0.041667f});

        tiles.push_back(snakeTile);
        reset();
    }

    shared_ptr<sSNAKE_TILE> Snake::addTile(const eDIRECTION aDirection) {
        if (!tiles.empty()) {
            auto snakeTile = make_shared<sSNAKE_TILE>();
            snakeTile->tile = make_shared<Cube>();
            glm::vec3 pos = {-19, 67, -23};

            const auto PrevIter = tiles.end()-1;

            if ((*(tiles.begin()))->direction != STOP) {
                pos = (*PrevIter)->tile->getPosition();
            } else {
                switch (aDirection) {
                    case LEFT:
                        if ((*PrevIter)->tile->getPosition().x - 2 >= -25) {
                            pos.x = (*PrevIter)->tile->getPosition().x - 2;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            return nullptr;
                        }
                        break;
                    case RIGHT:
                        if ((*PrevIter)->tile->getPosition().x + 2 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x + 2;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            return nullptr;
                        }
                        break;
                    case UP:
                        if ((*PrevIter)->tile->getPosition().y - 2 >= -25) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y - 2;
                        } else {
                            return nullptr;
                        }
                        break;
                    case DOWN:
                        if ((*PrevIter)->tile->getPosition().y + 2 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y + 2;
                        } else {
                            return nullptr;
                        }
                        break;
                    default:
                        break;
                }
            }

            // snakeTile->tile->setVirtualX(static_cast<int>(pos.x - (-23)) / 2 * 32 + 16);
            // snakeTile->tile->setVirtualY(static_cast<int>(pos.y - (-23)) / 2 * 32 + 16);
            snakeTile->tile->setPosition(pos);
            snakeTile->tile->setScale({0.041666667f, 0.041666667f, 0.041666667f});
            snakeTile->tile->setVisible(true);
            snakeTile->prevPauseDirection = NONE;
            tiles.push_back(snakeTile);

            return snakeTile;
        }

        return nullptr;
    }

    const vector<shared_ptr<sSNAKE_TILE>> &Snake::getItems() const {
        return tiles;
    }

    shared_ptr<Cube> Snake::getHeadTile() const {
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
        for (auto Iter = tiles.end() - 1; Iter != tiles.begin(); --Iter) {
            tiles.erase(Iter);
        }

        const shared_ptr<sSNAKE_TILE> snakeTile = (*tiles.begin());

        snakeTile->tile->setPosition({23, -3, -23}); // start pozice
        snakeTile->direction = STOP;
        snakeTile->prevPauseDirection = NONE;

        addTile(LEFT);
        addTile(LEFT);
        addTile(LEFT);
    }

} // ItemsDto