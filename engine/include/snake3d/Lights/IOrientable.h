#ifndef SNAKE3_IORIENTABLE_H
#define SNAKE3_IORIENTABLE_H

#include <glm/glm.hpp>

class IOrientable {
public:
    virtual ~IOrientable() = default;

    [[nodiscard]] glm::vec3 getDirection() const { return dir; }

    void setDirection(const glm::vec3 &dir) { this->dir = dir; }

protected:
    glm::vec3 dir = {};
};

#endif //SNAKE3_IORIENTABLE_H
