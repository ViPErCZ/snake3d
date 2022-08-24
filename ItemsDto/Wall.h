#ifndef SNAKE3_WALL_H
#define SNAKE3_WALL_H

#include "Cube.h"
#include "BaseContainer.h"

namespace ItemsDto {

    class Wall : public BaseContainer<Cube> {
    public:
        ~Wall();
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

#endif //SNAKE3_WALL_H
