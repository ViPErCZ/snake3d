#ifndef SNAKE3_WORLDENVIRONMENT_H
#define SNAKE3_WORLDENVIRONMENT_H

#include <memory>
#include "Environment.h"

namespace Tools {
    class WorldEnvironment {
        std::shared_ptr<Environment> environment;

    public:
        WorldEnvironment();

        [[nodiscard]] std::shared_ptr<Environment> getEnvironment() const;

        void setEnvironment(const std::shared_ptr<Environment> &environment);
    };
} // Tools

#endif //SNAKE3_WORLDENVIRONMENT_H
