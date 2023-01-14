#include "BaseItem.h"

namespace ItemsDto {
    BaseItem::BaseItem(): width(1), height(1), visible(true), alpha(1.0f), startFadeOut(false) {
        rotate[0].a = rotate[0].x = rotate[0].y = rotate[0].z = 0.0f;
        rotate[1].a = rotate[1].x = rotate[1].y = rotate[1].z = 0.0f;
        rotate[2].a = rotate[2].x = rotate[2].y = rotate[2].z = 0.0f;
    }

    const glm::vec3 &BaseItem::getPosition() const {
        return position;
    }

    void BaseItem::setPosition(const glm::vec3 &position) {
        BaseItem::position = position;
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

    const glm::vec3 &BaseItem::getZoom() const {
        return zoom;
    }

    void BaseItem::setZoom(const glm::vec3 &zoom) {
        BaseItem::zoom = zoom;
    }

    const glm::vec4 *BaseItem::getRotate() const {
        return rotate;
    }

    void BaseItem::setRotate(const glm::vec4 &rotateX, const glm::vec4 &rotateY, const glm::vec4 &rotateZ) {
        rotate[0] = rotateX;
        rotate[1] = rotateY;
        rotate[2] = rotateZ;
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