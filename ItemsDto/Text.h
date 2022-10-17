#ifndef SNAKE3_TEXT_H
#define SNAKE3_TEXT_H

#include "../stdafx.h"
#include "BaseItem.h"
#include <string>

using namespace std;

namespace ItemsDto {

    class Text : public BaseItem {
    public:
        explicit Text(string text);
        void setText(const string &text);
        void setColor(const glm::vec3 &color);
        [[nodiscard]] const string &getText() const;
        [[nodiscard]] const glm::vec3 &getColor() const;
        [[nodiscard]] unsigned int getFontSize() const;
        void setFontSize(unsigned int fontSize);
        [[nodiscard]] const string &getFontPath() const;
        void setFontPath(const string &fontPath);

    protected:
        string text;
        glm::vec3 color{};
        string fontPath;
        unsigned int fontSize{};
    };

} // ItemsDto

#endif //SNAKE3_TEXT_H
