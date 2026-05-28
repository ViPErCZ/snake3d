#ifndef SNAKE3_DIRECTIONALLIGHT_H
#define SNAKE3_DIRECTIONALLIGHT_H

#include <snake3d/Lights/OrientableLight.h>
#include <snake3d/Tools/Transform.h>
#include "Manager/ShaderProgram.h"

namespace Lights {
    class DirectionalLight : public OrientableLight {
        float shininess = 32.0f;

    public:
        void bind(const Manager::ShaderProgram *shader) const;
    };
} // Lights

#endif //SNAKE3_DIRECTIONALLIGHT_H
