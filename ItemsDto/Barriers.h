#ifndef SNAKE3_BARRIERS_H
#define SNAKE3_BARRIERS_H

#include "Cube.h"
#include "BaseContainer.h"
#include <vector>
#include "ObjWall.h"

using namespace std;

namespace ItemsDto {

    class Barriers final : public ObjWall {
    public:
        void init() override;
        [[nodiscard]] const vector<shared_ptr<Cube> > &getItems() const override;
        void createWall(int x, int y);
        void reset();
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;

    protected:
        static shared_ptr<Cube> wallFactory(const glm::vec3 &position);
        vector<shared_ptr<Cube> > walls;
    };

} // ItemsDto

#endif //SNAKE3_BARRIERS_H
