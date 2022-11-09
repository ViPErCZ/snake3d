#include "Barriers.h"

namespace ItemsDto {
    Barriers::~Barriers() {
        Barriers::release();
    }

    void Barriers::release() {
        if (!walls.empty()) {
            for (auto Iter = walls.begin(); Iter < walls.end(); Iter++) {
                delete (*Iter);
            }

            walls.clear();
        }
    }

    void Barriers::init() {
        walls.push_back(wallFactory({240.0, 336.0, 15.0}));
        walls.push_back(wallFactory({272.0, 336.0, 15.0}));
        walls.push_back(wallFactory({304.0, 336.0, 15.0}));
        walls.push_back(wallFactory({336.0, 336.0, 15.0}));
        walls.push_back(wallFactory({368.0, 336.0, 15.0}));
        walls.push_back(wallFactory({400.0, 336.0, 15.0}));
        walls.push_back(wallFactory({432.0, 336.0, 15.0}));
        walls.push_back(wallFactory({464.0, 336.0, 15.0}));
        walls.push_back(wallFactory({496.0, 336.0, 15.0}));
        walls.push_back(wallFactory({528.0, 336.0, 15.0}));
        walls.push_back(wallFactory({560.0, 336.0, 15.0}));
        walls.push_back(wallFactory({592.0, 336.0, 15.0}));
        walls.push_back(wallFactory({624.0, 336.0, 15.0}));

        walls.push_back(wallFactory({656.0, 336.0, 15.0}));
        walls.push_back(wallFactory({752.0, 336.0, 15.0}));
        walls.push_back(wallFactory({784.0, 336.0, 15.0}));
        walls.push_back(wallFactory({816.0, 336.0, 15.0}));
        walls.push_back(wallFactory({848.0, 336.0, 15.0}));
        walls.push_back(wallFactory({880.0, 336.0, 15.0}));
        walls.push_back(wallFactory({912.0, 336.0, 15.0}));
        walls.push_back(wallFactory({944.0, 336.0, 15.0}));
        walls.push_back(wallFactory({976.0, 336.0, 15.0}));
        walls.push_back(wallFactory({1008.0, 336.0, 15.0}));
        walls.push_back(wallFactory({1040.0, 336.0, 15.0}));
    }

    Cube *Barriers::wallFactory(const glm::vec3 &position) {
        auto wall = new Cube();
        wall->setPosition(position);
        wall->setVirtualX((((int)(position.x - (-23)) / 2) * 32));
        wall->setVirtualY((((int)(position.y - (-23)) / 2) * 32));
        wall->setVisible(true);

        return wall;
    }

    const vector<Cube *> &Barriers::getItems() const {
        return walls;
    }

    int Barriers::getMaxX() const {
        return 0;
    }

    int Barriers::getMaxY() const {
        return 0;
    }

    int Barriers::getMinX() const {
        return 0;
    }

    int Barriers::getMinY() const {
        return 0;
    }

    void Barriers::createWall(int x, int y) {
        walls.push_back(wallFactory({(float)x, (float)y, -23.0}));
    }

    void Barriers::reset() {
        release();
    }

} // ItemsDto