#ifndef SNAKE3_SHADERMANAGER_H
#define SNAKE3_SHADERMANAGER_H

#include "../stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

using namespace std;

namespace Manager {

    class ShaderManager {
    public:
        explicit ShaderManager(GLuint id);
        void use() const;
        void setBool(const string &name, bool value) const;
        void setInt(const string &name, int value) const;
        void setFloat(const string &name, float value) const;
        void setVec2(const string &name, const glm::vec2 &value) const;
        void setVec2(const string &name, float x, float y) const;
        void setVec3(const string &name, const glm::vec3 &value) const;
        void setVec3(const string &name, float x, float y, float z) const;
        void setVec4(const string &name, const glm::vec4 &value) const;
        void setVec4(const string &name, float x, float y, float z, float w) const;
        void setMat2(const string &name, const glm::mat2 &mat) const;
        void setMat3(const string &name, const glm::mat3 &mat) const;
        void setMat4(const string &name, const glm::mat4 &mat) const;
    protected:
        GLuint id;
    };

} // Manager

#endif //SNAKE3_SHADERMANAGER_H
