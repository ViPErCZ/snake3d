#include "../Tools/Vector3i.h"

namespace Node3D {
    Vector3i::Vector3i(const int x, const int y, const int z): x(x), y(y), z(z) {
    }

    Vector3i::Vector3i(): x(0), y(0), z(0) {}
} // Node3D