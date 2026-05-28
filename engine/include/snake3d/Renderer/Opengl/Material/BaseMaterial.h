#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

#include <snake3d/Renderer/Opengl/Material/Interface/BlendingInterface.h>

namespace Material {
    class BaseMaterial : public BlendingInterface {
    public:
        ~BaseMaterial() override = default;

        [[nodiscard]] virtual std::shared_ptr<BaseMaterial> clone() const = 0;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
