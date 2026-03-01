#include "Named.h"

namespace Node3D {
    void Named::setName(const string &name) {
        this->name = name;
    }

    string Named::getName() {
        return name;
    }
} // Node3D
