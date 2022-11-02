#ifndef SNAKE3_OBJWALL_H
#define SNAKE3_OBJWALL_H

#include "BaseContainer.h"
#include "Cube.h"

namespace ItemsDto {

    class ObjWall : public BaseContainer<Cube> {
    public:
        ~ObjWall();
        void init() override;
        [[nodiscard]] const vector<Cube *> &getItems() const override;
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;
    protected:
        void release() override;
        vector<Cube*> walls;
    };

} // ItemsDto

#endif //SNAKE3_OBJWALL_H
