#include <snake3d/Tools/Environment.h>

namespace Tools {
    Environment::Environment() {
        ambientLight = AmbientLight();
        ambientLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
        ambientLight.intensity = 1;
    }

    AmbientLight Environment::getAmbientLight() const {
        return ambientLight;
    }
} // Tools