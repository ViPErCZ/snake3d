#include "BaseMaterial.h"

namespace Material {
    void BaseMaterial::setBlending(const Blending blending) {
        this->blending = blending;
    }

    Blending BaseMaterial::getBlending() const {
        return this->blending;
    }
} // Material
