#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

#include "Interface/BlendingInterface.h"

using namespace std;

namespace Material {
    class BaseMaterial : public BlendingInterface {
    public:
        ~BaseMaterial() override = default;

        [[nodiscard]] virtual shared_ptr<BaseMaterial> clone() const = 0;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
