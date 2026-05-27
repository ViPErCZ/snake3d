#include "BlendingInterface.h"

using namespace Tools;

namespace Material {
    void BlendingInterface::setBlending(const Blending blending) {
        this->blending = blending;
    }

    Blending BlendingInterface::getBlending() const {
        return blending;
    }
} // Material