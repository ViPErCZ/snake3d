#ifndef SNAKE3_BASEITEM_H
#define SNAKE3_BASEITEM_H

#include "../stdafx.h"
#include <iostream>
#include <glm/glm.hpp>

using namespace std;

namespace ItemsDto {
    class BaseItem {
    public:
        BaseItem();

        void setPosition(const glm::vec3 &position);
        void setWorldMatrix(const glm::mat4 &matrix);
        [[nodiscard]] const glm::vec3 &getPosition() const;
        [[nodiscard]] glm::mat4 getWorldMatrix() const;
        [[nodiscard]] bool isVisible() const;
        void setVisible(bool visible);
        void toggleVisible();
        [[nodiscard]] GLfloat getWidth() const;
        void setWidth(GLfloat width);
        [[nodiscard]] GLfloat getHeight() const;
        void setHeight(GLfloat height);
        [[nodiscard]] const glm::vec3 &getZoom() const;
        void setZoom(const glm::vec3 &zoom);
        [[nodiscard]] const glm::vec4 *getRotate() const;
        void setRotate(const glm::vec4 &rotateX, const glm::vec4 &rotateY, const glm::vec4 &rotateZ);
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
        glm::mat4 worldMatrix{};
        glm::vec3 position{};
        glm::vec3 zoom{1.0f, 1.0f, 1.0f};
        glm::vec4 rotate[3]{};
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
