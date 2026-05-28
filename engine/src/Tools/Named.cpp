#include <snake3d/Tools/Named.h>

using namespace std;

namespace Node3D {
    void Named::setName(const string &name) {
        this->name = name;
    }

    string Named::getName() {
        return name;
    }
} // Node3D
