#ifndef SNAKE3_INTERFACE_BLENDING_H
#define SNAKE3_INTERFACE_BLENDING_H

#include <snake3d/Tools/Blending.h>

namespace Material {
    class BlendingInterface {
    public:
        virtual ~BlendingInterface() = default;

        void setBlending(Tools::Blending blending);

        [[nodiscard]] Tools::Blending getBlending() const;

    protected:
        Tools::Blending blending = Tools::Blending::Opaque;
    };
} // Material

#endif //SNAKE3_INTERFACE_BLENDING_H