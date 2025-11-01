#ifndef SNAKE3_BASEITEM_H
#define SNAKE3_BASEITEM_H

#include "../stdafx.h"
#include <iostream>
#include "Transform.h"
#include "Vector3i.h"

using namespace Node3D;
using namespace std;

namespace ItemsDto {
    class BaseItem : public Transform, public Vector3i {
    public:
        BaseItem();

        ~BaseItem() override = default;

        [[nodiscard]] bool isVisible() const;
        void setVisible(bool visible);
        void toggleVisible();
        [[nodiscard]] GLfloat getWidth() const;
        void setWidth(GLfloat width);
        [[nodiscard]] GLfloat getHeight() const;
        void setHeight(GLfloat height);
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
        double lastTime{};
        bool startFadeOut;
        bool startFadeIn{};
        float alpha;
    };

} // ItemsDto

#endif //SNAKE3_BASEITEM_H
