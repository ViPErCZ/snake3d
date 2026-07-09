#ifndef SNAKE3_ORIENTABLELIGHT_H
#define SNAKE3_ORIENTABLELIGHT_H

#include <snake3d/Lights/IOrientable.h>
#include <snake3d/Lights/Light.h>

namespace Lights {
    class OrientableLight : public Light, public IOrientable {
    public:
        ~OrientableLight() override = default;
    };
}

#endif //SNAKE3_ORIENTABLELIGHT_H