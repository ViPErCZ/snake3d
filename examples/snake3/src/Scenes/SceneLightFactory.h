#ifndef SNAKE3_SCENELIGHTFACTORY_H
#define SNAKE3_SCENELIGHTFACTORY_H

#include <memory>
#include <vector>

#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"

namespace Scenes {
    struct SceneLights {
        std::vector<std::shared_ptr<Lights::SpotLight>> spotLights;
        std::vector<std::shared_ptr<Lights::PointLight>> pointLights;
    };

    class SceneLightFactory {
    public:
        static SceneLights create();
    };
}

#endif //SNAKE3_SCENELIGHTFACTORY_H
