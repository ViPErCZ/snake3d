#include "Barriers.h"

namespace ItemsDto {
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

    shared_ptr<Cube> Barriers::wallFactory(const glm::vec3 &position) {
        auto wall = make_shared<Cube>();
        wall->setPosition(position);
        wall->setVirtualX(static_cast<int>(position.x - (-23)) / 2 * 32);
        wall->setVirtualY(static_cast<int>(position.y - (-23)) / 2 * 32);
        wall->setVisible(true);

        return wall;
    }

    const vector<shared_ptr<Cube> > &Barriers::getItems() const {
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

    void Barriers::createWall(const int x, const int y) {
        walls.push_back(wallFactory({static_cast<float>(x), static_cast<float>(y), -23.0}));
    }

    void Barriers::reset() {
        walls.clear();
    }

} // ItemsDto