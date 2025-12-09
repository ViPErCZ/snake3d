#ifndef SNAKE3_BASEMATERIAL_H
#define SNAKE3_BASEMATERIAL_H

#include <memory>

#include "../../../Tools/Blending.h"

using namespace Tools;
using namespace std;

namespace Material {
    class BaseMaterial {
    public:
        virtual ~BaseMaterial() = default;

        [[nodiscard]] virtual shared_ptr<BaseMaterial> clone() const = 0;

        void setBlending(Blending blending);

        [[nodiscard]] Blending getBlending() const;

    protected:
        Blending blending = Blending::Opaque;
    };
} // Material

#endif //SNAKE3_BASEMATERIAL_H
