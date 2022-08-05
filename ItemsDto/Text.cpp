#include "Text.h"

#include <utility>

namespace ItemsDto {
    Text::Text(string text) : text(std::move(text)) {
        font = GLUT_BITMAP_8_BY_13;
    }

    void Text::setText(const string &text) {
        Text::text = text;
    }

    void Text::setColor(const sCOLOR &color) {
        Text::color = color;
    }

    const string &Text::getText() const {
        return text;
    }

    const sCOLOR &Text::getColor() const {
        return color;
    }

    void *Text::getFont() const {
        return font;
    }

    void Text::setFont(void *font) {
        Text::font = font;
    }
} // ItemsDto