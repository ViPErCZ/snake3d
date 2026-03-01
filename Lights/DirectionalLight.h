#ifndef SNAKE3_DIRECTIONALLIGHT_H
#define SNAKE3_DIRECTIONALLIGHT_H

#include "OrientableLight.h"
#include "../ItemsDto/Transform.h"
#include "../Manager/ShaderManager.h"

using namespace Node3D;
using namespace Manager;

namespace Lights {
    class DirectionalLight : public OrientableLight {
        float shininess = 32.0f;

    public:
        void bind(const ShaderManager *shader) const;
    };
} // Lights

#endif //SNAKE3_DIRECTIONALLIGHT_H
