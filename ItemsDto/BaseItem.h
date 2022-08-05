#ifndef SNAKE3_BASEITEM_H
#define SNAKE3_BASEITEM_H

#include "../stdafx.h"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>

using namespace std;

namespace ItemsDto {
    class BaseItem {
    public:
        BaseItem();

        void addTexture(GLuint index);
        GLuint findTexture(GLuint index);
        GLuint getPrimaryTexture();
        void setPosition(const glm::vec3 &position);
        [[nodiscard]] const glm::vec3 &getPosition() const;
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
        [[nodiscard]] bool isBlend() const;
        void setBlend(bool blend);
        [[nodiscard]] GLuint getProgram() const;
        void setProgram(GLuint program);

    protected:
        vector<GLuint> textures;
        glm::vec3 position{};
        glm::vec3 zoom{};
        glm::vec4 rotate[3]{};
        bool visible{};
        bool blend;
        GLfloat width;
        GLfloat height;
        GLuint program;
    };

} // ItemsDto

#endif //SNAKE3_BASEITEM_H