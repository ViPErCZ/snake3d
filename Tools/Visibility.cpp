#include "../Tools/Visibility.h"

namespace Node3D {
    bool Visibility::isVisible() const {
        return visible;
    }

    void Visibility::setVisible(const bool visible) {
        this->visible = visible;
    }
} // Node3D