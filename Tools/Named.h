#ifndef SNAKE3_NAMED_H
#define SNAKE3_NAMED_H

#include <string>

namespace Node3D {
    class Named {
    public:
        void setName(const std::string &name);

        [[nodiscard]] std::string getName();

    protected:
        std::string name;
    };
} // Node3D

#endif //SNAKE3_NAMED_H