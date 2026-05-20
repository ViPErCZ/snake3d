#include "MaterialPlaceholder.h"

namespace Manager {
    bool isKnownMaterialPlaceholder(const std::string_view marker) {
        for (const auto slot : kAllSlots) {
            if (slotMarker(slot) == marker) return true;
        }
        return false;
    }
} // Manager
