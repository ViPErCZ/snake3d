#ifndef SNAKE3_LABELSETTINGS_H
#define SNAKE3_LABELSETTINGS_H

#include <memory>

#include "Font.h"

using namespace std;

namespace Material {
    class LabelSettings {
    public:
        explicit LabelSettings(const std::shared_ptr<Font> &font);
        [[nodiscard]] shared_ptr<Font> getFont() const;
        // glm::vec4 getColor() const;
        // void setColor(const glm::vec4 &color);
        [[nodiscard]] float getLetterSpacing() const;
        // void setLetterSpacing(float letterSpacing);
        // float getLineSpacing() const;
        // void setLineSpacing(float lineSpacing);
        // bool isWordWrap() const;
        // void setWordWrap(bool wordWrap);
    private:
        std::shared_ptr<Font> font;
        glm::vec4 color = glm::vec4(1.0f);
        float letterSpacing = 0.0f;
        float lineSpacing = 1.0f;
        bool wordWrap = false;
    };
} // Material

#endif //SNAKE3_LABELSETTINGS_H