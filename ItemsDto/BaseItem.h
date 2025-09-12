#ifndef SNAKE3_BASEITEM_H
#define SNAKE3_BASEITEM_H

#include "../stdafx.h"
#include <iostream>

#include "Transform.h"

using namespace Node3D;
using namespace std;

namespace ItemsDto {
    class BaseItem : public Transform{
    public:
        BaseItem();

        [[nodiscard]] bool isVisible() const;
        void setVisible(bool visible);
        void toggleVisible();
        [[nodiscard]] GLfloat getWidth() const;
        void setWidth(GLfloat width);
        [[nodiscard]] GLfloat getHeight() const;
        void setHeight(GLfloat height);
        [[nodiscard]] int getVirtualX() const;
        void setVirtualX(int virtualX);
        [[nodiscard]] int getVirtualY() const;
        void setVirtualY(int virtualY);
        void fadeOut();
        void fadeIn();
        [[nodiscard]] float getAlpha() const;
        void setAlpha(float alpha);
        void fadeStep(float FADE_STEP = 0.01f);
        [[nodiscard]] bool isStartFade() const;

    protected:
        bool visible{};
        GLfloat width;
        GLfloat height;
        int virtual_X{};
        int virtual_Y{};
        double lastTime{};
        bool startFadeOut;
        bool startFadeIn{};
        float alpha;
    };

} // ItemsDto

#endif //SNAKE3_BASEITEM_H
