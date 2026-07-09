#include <snake3d/Tools/Visibility.h>

namespace Node3D {
    bool Visibility::isVisible() const {
        return visible;
    }

    void Visibility::setVisible(const bool visible) {
        if (debugLock) return;  // Inspector override active -- ignore gameplay.
        this->visible = visible;
    }

    void Visibility::setVisibleForced(const bool visible) {
        this->visible = visible;
        debugLock = true;
    }

    void Visibility::unlockVisible() {
        debugLock = false;
    }
} // Node3D