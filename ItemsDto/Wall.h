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
        [[nodiscard]] float getMaxX() const override;
        [[nodiscard]] float getMaxY() const override;
        [[nodiscard]] float getMinX() const override;
        [[nodiscard]] float getMinY() const override;

    protected:
        void release() override;
        vector<Cube*> walls;
    };

} // ItemsDto

#endif //SNAKE3_WALL_H
