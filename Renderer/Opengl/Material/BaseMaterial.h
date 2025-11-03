#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

using namespace std;

namespace Material {
    class BaseMaterial {
    public:
        virtual ~BaseMaterial() = default;

        [[nodiscard]] virtual shared_ptr<BaseMaterial> clone() const = 0;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
