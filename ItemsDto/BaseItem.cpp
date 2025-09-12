#include "BaseItem.h"

#include <glm/ext/matrix_transform.hpp>

namespace ItemsDto {
    BaseItem::BaseItem(): visible(true), width(1), height(1), startFadeOut(false), alpha(1.0f) {
        rotate[0].w = rotate[0].x = rotate[0].y = rotate[0].z = 0.0f;
        rotate[1].w = rotate[1].x = rotate[1].y = rotate[1].z = 0.0f;
        rotate[2].w = rotate[2].x = rotate[2].y = rotate[2].z = 0.0f;
    }

    bool BaseItem::isVisible() const {
        return visible;
    }

    void BaseItem::setVisible(bool visible) {
        BaseItem::visible = visible;
        alpha = 1.0f;
        startFadeOut = false;
        startFadeIn = false;
    }

    GLfloat BaseItem::getWidth() const {
        return width;
    }

    void BaseItem::setWidth(GLfloat width) {
        BaseItem::width = width;
    }

    GLfloat BaseItem::getHeight() const {
        return height;
    }

    void BaseItem::setHeight(GLfloat height) {
        BaseItem::height = height;
    }

    void BaseItem::toggleVisible() {
        this->visible = !this->visible;
    }

    int BaseItem::getVirtualX() const {
        return virtual_X;
    }

    void BaseItem::setVirtualX(int virtualX) {
        virtual_X = virtualX;
    }

    int BaseItem::getVirtualY() const {
        return virtual_Y;
    }

    void BaseItem::setVirtualY(int virtualY) {
        virtual_Y = virtualY;
    }

    void BaseItem::fadeOut() {
        if (alpha == 1.0) {
            startFadeOut = true;
        }
    }

    void BaseItem::fadeIn() {
        if (alpha == 0.0) {
            startFadeIn = true;
        }
    }

    float BaseItem::getAlpha() const {
        return alpha;
    }

    bool BaseItem::isStartFade() const {
        return startFadeOut || startFadeIn;
    }

    void BaseItem::setAlpha(float alpha) {
        BaseItem::alpha = alpha;
    }

    void BaseItem::fadeStep(const float FADE_STEP) {
        double now = glfwGetTime();
        if (now > lastTime + 0.0001) {
            lastTime = now;
            if (startFadeOut && alpha > 0) {
                alpha -= FADE_STEP;

                return;
            } else if (startFadeIn && alpha < 1) {
                alpha += FADE_STEP;

                return;
            }

            visible = !startFadeOut;
            startFadeOut = false;
            startFadeIn = false;
        }
    }

} // ItemsDto