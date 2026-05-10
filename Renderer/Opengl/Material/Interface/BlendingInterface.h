#ifndef SNAKE3_INTERFACE_BLENDING_H
#define SNAKE3_INTERFACE_BLENDING_H

#include "../../../../Tools/Blending.h"

using namespace Tools;

namespace Material {
    class BlendingInterface {
    public:
        virtual ~BlendingInterface() = default;

        void setBlending(Blending blending);

        [[nodiscard]] Blending getBlending() const;

    protected:
        Blending blending = Blending::Opaque;
    };
} // Material

#endif //SNAKE3_INTERFACE_BLENDING_H