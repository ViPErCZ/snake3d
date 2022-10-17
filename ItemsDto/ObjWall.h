#ifndef SNAKE3_OBJWALL_H
#define SNAKE3_OBJWALL_H

#include "ObjItem.h"
#include "BaseContainer.h"

namespace ItemsDto {

    class ObjWall : public BaseContainer<ObjItem> {
    public:
        ~ObjWall();
        void init() override;
        [[nodiscard]] const vector<ObjItem *> &getItems() const override;
        [[nodiscard]] int getMaxX() const override;
        [[nodiscard]] int getMaxY() const override;
        [[nodiscard]] int getMinX() const override;
        [[nodiscard]] int getMinY() const override;
    protected:
        void release() override;
        vector<ObjItem*> walls;
    };

} // ItemsDto

#endif //SNAKE3_OBJWALL_H
