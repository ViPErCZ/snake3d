#ifndef SNAKE3_BARRIERS_H
#define SNAKE3_BARRIERS_H

#include "Cube.h"
#include "BaseContainer.h"
#include "ObjItem.h"
#include "ObjWall.h"

namespace ItemsDto {

    class Barriers : public BaseContainer<ObjItem> {
    public:
        ~Barriers();
        void init() override;
        [[nodiscard]] const vector<ObjItem *> &getItems() const override;
        void createWall(int x, int y);
        void reset();
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;

    protected:
        static ObjItem* wallFactory(const glm::vec3 &position);
        void release() override;
        vector<ObjItem*> walls;
    };

} // ItemsDto

#endif //SNAKE3_BARRIERS_H
