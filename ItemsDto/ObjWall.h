#ifndef SNAKE3_OBJWALL_H
#define SNAKE3_OBJWALL_H

#include "Wall.h"
#include "ObjItem.h"

namespace ItemsDto {

    class ObjWall : public BaseContainer<ObjItem> {
    public:
        ~ObjWall();
        void init() override;
        [[nodiscard]] const vector<ObjItem *> &getItems() const override;
        [[nodiscard]] float getMaxX() const override;
        [[nodiscard]] float getMaxY() const override;
        [[nodiscard]] float getMinX() const override;
        [[nodiscard]] float getMinY() const override;
    protected:
        void release() override;
        vector<ObjItem*> walls;
    };

} // ItemsDto

#endif //SNAKE3_OBJWALL_H
