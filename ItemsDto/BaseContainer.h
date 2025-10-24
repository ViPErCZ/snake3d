#ifndef SNAKE3_BASECONTAINER_H
#define SNAKE3_BASECONTAINER_H

#include "BaseContainerInterface.h"
#include <vector>
#include <memory>

using namespace std;

namespace ItemsDto {
    template<class ParentType>
    class BaseContainer : public BaseContainerInterface {
    public:
        virtual ~BaseContainer() = default;
        virtual void init() = 0;
        [[nodiscard]] virtual const vector<shared_ptr<ParentType>> &getItems() const = 0;
        [[nodiscard]] virtual int getMaxX() const = 0;
        [[nodiscard]] virtual int getMaxY() const = 0;
        [[nodiscard]] virtual int getMinX() const = 0;
        [[nodiscard]] virtual int getMinY() const = 0;
    protected:
        int maxX{};
        int maxY{};
        int minX{};
        int minY{};
    };

} // ItemsDto

#endif //SNAKE3_BASECONTAINER_H
