#ifndef SNAKE_OBJSPRITE_H
#define SNAKE_OBJSPRITE_H

#include "Sprite.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

using namespace std;

class ObjSprite : public Sprite {
public:
    ObjSprite();
    bool Load(char *filename);
    bool Release() override;
    void Render() override;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2>  uvs;
    std::vector<glm::vec3> normals;

    void setPos(Vector2f pos);
    void setPos();
};


#endif //SNAKE_OBJSPRITE_H
