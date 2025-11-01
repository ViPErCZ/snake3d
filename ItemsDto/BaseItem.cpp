#include "BaseItem.h"
#include <glm/ext/matrix_transform.hpp>

namespace ItemsDto {
    BaseItem::BaseItem() : visible(true), width(1), height(1), startFadeOut(false), alpha(1.0f) {
    }

    bool BaseItem::isVisible() const {
        return visible;
    }

    void BaseItem::setVisible(const bool visible) {
        BaseItem::visible = visible;
        alpha = 1.0f;
        startFadeOut = false;
        startFadeIn = false;
    }

    GLfloat BaseItem::getWidth() const {
        return width;
    }

    void BaseItem::setWidth(const GLfloat width) {
        BaseItem::width = width;
    }

    GLfloat BaseItem::getHeight() const {
        return height;
    }

    void BaseItem::setHeight(const GLfloat height) {
        BaseItem::height = height;
    }

    void BaseItem::toggleVisible() {
        this->visible = !this->visible;
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

    void BaseItem::setAlpha(const float alpha) {
        BaseItem::alpha = alpha;
    }

    void BaseItem::fadeStep(const float FADE_STEP) {
        const double now = glfwGetTime();
        if (now > lastTime + 0.0001) {
            lastTime = now;
            if (startFadeOut && alpha > 0) {
                alpha -= FADE_STEP;

                return;
            }
            if (startFadeIn && alpha < 1) {
                alpha += FADE_STEP;

                return;
            }

            visible = !startFadeOut;
            startFadeOut = false;
            startFadeIn = false;
        }
    }
} // ItemsDto
