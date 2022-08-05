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
        snakeTile->tile->addTexture(6);
        glm::vec3 zoom = {0, 0, 0};
        zoom.x = 15.0;
        zoom.y = 15.0;
        zoom.z = 15.0;
        snakeTile->tile->setZoom(zoom);
        snakeTile->tile->setVisible(true);
        snakeTile->alpha = 1.0;

        tiles.push_back(snakeTile);
        reset();
    }

    void Snake::addTile(eDIRECTION aDirection) {
        if (!tiles.empty()) {
            auto *snakeTile = new sSNAKE_TILE;
            snakeTile->tile = new Cube();
            glm::vec3 pos = {0, 0, 15};
            glm::vec3 zoom = {0, 0, 0};

            auto PrevIter = tiles.end()-1;

            if ((*(tiles.begin()))->direction != STOP) {
                pos = (*PrevIter)->tile->getPosition();
            } else {
                switch (aDirection) {
                    case LEFT:
                        if ((*PrevIter)->tile->getPosition().x - 32 >= 16) {
                            pos.x = (*PrevIter)->tile->getPosition().x - 32;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case RIGHT:
                        if ((*PrevIter)->tile->getPosition().x + 32 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x + 32;
                            pos.y = (*PrevIter)->tile->getPosition().y;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case UP:
                        if ((*PrevIter)->tile->getPosition().y - 32 >= 16) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y - 32;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    case DOWN:
                        if ((*PrevIter)->tile->getPosition().y + 32 <= 752) {
                            pos.x = (*PrevIter)->tile->getPosition().x;
                            pos.y = (*PrevIter)->tile->getPosition().y + 32;
                        } else {
                            delete snakeTile;
                            return;
                        }
                        break;
                    default:
                        break;
                }
            }

            snakeTile->tile->addTexture(7);
            snakeTile->tile->setPosition(pos);
            snakeTile->tile->setVisible(true);
            zoom.x = 15.0;
            zoom.y = 15.0;
            zoom.z = 15.0;
            snakeTile->tile->setZoom(zoom);
            snakeTile->prevPauseDirection = NONE;
            snakeTile->alpha = 1.0;
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

    float Snake::getMaxX() const {
        return 0;
    }

    float Snake::getMaxY() const {
        return 0;
    }

    float Snake::getMinX() const {
        return 0;
    }

    float Snake::getMinY() const {
        return 0;
    }

    void Snake::reset() {
        for (auto Iter = tiles.end() - 1; Iter != tiles.begin(); Iter--) {
            delete (*Iter)->tile;
            delete (*Iter);

            tiles.erase(Iter);
        }

        sSNAKE_TILE* snakeTile = (*tiles.begin());

        glm::vec3 pos = {0, 0, 0};
        pos.x = 368;
        pos.y = 464;
        pos.z = 15;
        snakeTile->tile->setPosition(pos);
        snakeTile->tile->setVisible(true);
        snakeTile->direction = STOP;
        snakeTile->prevPauseDirection = NONE;

        addTile(LEFT);
        addTile(LEFT);
        addTile(LEFT);
    }

} // ItemsDto