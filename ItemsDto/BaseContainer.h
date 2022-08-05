#ifndef SNAKE3_BASECONTAINER_H
#define SNAKE3_BASECONTAINER_H

#include "BaseItem.h"
#include "BaseContainerInterface.h"

namespace ItemsDto {
    template<class ParentType>
    class BaseContainer : public BaseContainerInterface {
    public:
        virtual void init() = 0;
        [[nodiscard]] virtual const vector<ParentType *> &getItems() const = 0;
        [[nodiscard]] virtual float getMaxX() const = 0;
        [[nodiscard]] virtual float getMaxY() const = 0;
        [[nodiscard]] virtual float getMinX() const = 0;
        [[nodiscard]] virtual float getMinY() const = 0;
    protected:
        virtual void release() = 0;
        float maxX{};
        float maxY{};
        float minX{};
        float minY{};
    };

} // ItemsDto

#endif //SNAKE3_BASECONTAINER_H
