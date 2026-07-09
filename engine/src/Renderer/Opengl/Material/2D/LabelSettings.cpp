#include <snake3d/Renderer/Opengl/Material/2D/LabelSettings.h>

using namespace std;

namespace Material {
    LabelSettings::LabelSettings(const std::shared_ptr<Font> &font) : font(font), color(glm::vec3(1.0f)) {
    }

    shared_ptr<Font> LabelSettings::getFont() const {
        return font;
    }

    glm::vec3 LabelSettings::getColor() const {
        return color;
    }

    void LabelSettings::setColor(const glm::vec3 &color) {
        this->color = color;
    }

    float LabelSettings::getLetterSpacing() const {
        return letterSpacing;
    }
} // Material