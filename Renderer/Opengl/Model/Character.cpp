#include "Character.h"

namespace Model {
    unsigned int oCharacter::getTextureId() const {
        return textureId;
    }

    void oCharacter::setTextureId(unsigned int textureId) {
        oCharacter::textureId = textureId;
    }

    const glm::ivec2 &oCharacter::getSize() const {
        return size;
    }

    void oCharacter::setSize(const glm::ivec2 &size) {
        oCharacter::size = size;
    }

    const glm::ivec2 &oCharacter::getBearing() const {
        return bearing;
    }

    void oCharacter::setBearing(const glm::ivec2 &bearing) {
        oCharacter::bearing = bearing;
    }

    unsigned int oCharacter::getAdvance() const {
        return advance;
    }

    void oCharacter::setAdvance(unsigned int advance) {
        oCharacter::advance = advance;
    }
} // Model