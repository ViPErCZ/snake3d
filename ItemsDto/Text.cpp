#include "Text.h"

#include <utility>

namespace ItemsDto {
    Text::Text(string text) : text(std::move(text)) {}

    void Text::setText(const string &text) {
        Text::text = text;
    }

    void Text::setColor(const glm::vec3 &color) {
        Text::color = color;
    }

    const string &Text::getText() const {
        return text;
    }

    const glm::vec3 &Text::getColor() const {
        return color;
    }

    unsigned int Text::getFontSize() const {
        return fontSize;
    }

    void Text::setFontSize(unsigned int fontSize) {
        Text::fontSize = fontSize;
    }

    const string &Text::getFontPath() const {
        return fontPath;
    }

    void Text::setFontPath(const string &fontPath) {
        Text::fontPath = fontPath;
    }
}