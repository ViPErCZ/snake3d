#include "WorldEnvironment.h"

namespace Tools {
    std::shared_ptr<Environment> WorldEnvironment::getEnvironment() const {
        return environment;
    }

    void WorldEnvironment::setEnvironment(const std::shared_ptr<Environment> &environment) {
        this->environment = environment;
    }
} // Tools