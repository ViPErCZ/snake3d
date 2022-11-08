#ifndef SNAKE3_BARRIERS_H
#define SNAKE3_BARRIERS_H

#include "Cube.h"
#include "BaseContainer.h"
#include "ObjWall.h"
#include <vector>

using namespace std;

namespace ItemsDto {

    class Barriers : public BaseContainer<Cube> {
    public:
        ~Barriers();
        void init() override;
        [[nodiscard]] const vector<Cube *> &getItems() const override;
        void createWall(int x, int y);
        void reset();
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;

    protected:
        static Cube* wallFactory(const glm::vec3 &position);
        void release() override;
        vector<Cube*> walls;
    };

} // ItemsDto

#endif //SNAKE3_BARRIERS_H
