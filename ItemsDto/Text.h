#ifndef SNAKE3_TEXT_H
#define SNAKE3_TEXT_H

#include "../stdafx.h"
#include "BaseItem.h"
#include <string>

using namespace std;

namespace ItemsDto {

    struct sCOLOR {
        GLfloat r, g, b;
    };

    class Text : public BaseItem {
    public:
        explicit Text(string text);
        void setText(const string &text);
        void setColor(const sCOLOR &color);
        [[nodiscard]] const string &getText() const;
        [[nodiscard]] const sCOLOR &getColor() const;
        [[nodiscard]] void *getFont() const;
        void setFont(void *font);

    protected:
        string text;
        sCOLOR color{};
        void* font;
    };

} // ItemsDto

#endif //SNAKE3_TEXT_H
