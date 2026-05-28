#ifndef SNAKE3_ENVIRONMENT_H
#define SNAKE3_ENVIRONMENT_H

#include <snake3d/Tools/Clear.h>
#include <glm/vec3.hpp>

namespace Tools {
    struct AmbientLight {
        glm::vec3 color;
        float intensity;
    };

    struct Background {
        Clear mode;
    };

    class Environment {
        // background
        // -- mode
        // -- color

        AmbientLight ambientLight{};

    public:
        Environment();

        ~Environment() = default;

        [[nodiscard]] AmbientLight getAmbientLight() const;
    };
} // Tools

#endif //SNAKE3_ENVIRONMENT_H
