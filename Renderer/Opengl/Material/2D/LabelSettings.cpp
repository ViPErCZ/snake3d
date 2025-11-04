#include "LabelSettings.h"

namespace Material {
    LabelSettings::LabelSettings(const std::shared_ptr<Font> &font) : font(font) {
    }

    shared_ptr<Font> LabelSettings::getFont() const {
        return font;
    }

    float LabelSettings::getLetterSpacing() const {
        return letterSpacing;
    }
} // Material