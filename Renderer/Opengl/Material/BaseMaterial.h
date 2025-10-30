#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

namespace Material {
    class BaseMaterial {
    public:
        virtual ~BaseMaterial() = default;

        [[nodiscard]] virtual std::shared_ptr<BaseMaterial> clone() const = 0;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
