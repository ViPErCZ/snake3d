#include "Character.h"

namespace Model {
    unsigned int Character::getTextureId() const {
        return textureId;
    }

    void Character::setTextureId(unsigned int textureId) {
        Character::textureId = textureId;
    }

    const glm::ivec2 &Character::getSize() const {
        return size;
    }

    void Character::setSize(const glm::ivec2 &size) {
        Character::size = size;
    }

    const glm::ivec2 &Character::getBearing() const {
        return bearing;
    }

    void Character::setBearing(const glm::ivec2 &bearing) {
        Character::bearing = bearing;
    }

    unsigned int Character::getAdvance() const {
        return advance;
    }

    void Character::setAdvance(unsigned int advance) {
        Character::advance = advance;
    }
} // Model