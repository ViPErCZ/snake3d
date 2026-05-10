#ifndef SNAKE3_ORIENTABLELIGHT_H
#define SNAKE3_ORIENTABLELIGHT_H

#include "IOrientable.h"
#include "Light.h"

namespace Lights {
    class OrientableLight : public Light, public IOrientable {
    public:
        ~OrientableLight() override = default;
    };
}

#endif //SNAKE3_ORIENTABLELIGHT_H