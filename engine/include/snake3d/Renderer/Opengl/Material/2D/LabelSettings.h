#ifndef SNAKE3_LABELSETTINGS_H
#define SNAKE3_LABELSETTINGS_H

#include <memory>

#include <snake3d/Renderer/Opengl/Material/2D/Font.h>

namespace Material {
    class LabelSettings {
    public:
        explicit LabelSettings(const std::shared_ptr<Font> &font);
        [[nodiscard]] std::shared_ptr<Font> getFont() const;
        [[nodiscard]] glm::vec3 getColor() const;
        void setColor(const glm::vec3 &color);
        [[nodiscard]] float getLetterSpacing() const;
        // void setLetterSpacing(float letterSpacing);
        // float getLineSpacing() const;
        // void setLineSpacing(float lineSpacing);
        // bool isWordWrap() const;
        // void setWordWrap(bool wordWrap);
    private:
        std::shared_ptr<Font> font;
        glm::vec3 color;
        float letterSpacing = 0.0f;
        float lineSpacing = 1.0f;
        bool wordWrap = false;
    };
} // Material

#endif //SNAKE3_LABELSETTINGS_H