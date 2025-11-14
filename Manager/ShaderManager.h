#ifndef SNAKE3_SHADERMANAGER_H
#define SNAKE3_SHADERMANAGER_H

#include "../stdafx.h"
#include <string>
#include <variant>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

using UniformValue = std::variant<
    bool,
    int,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat2,
    glm::mat3,
    glm::mat4
>;

namespace Manager {

    class ShaderManager {
    public:
        explicit ShaderManager(GLuint id);
        void use() const;
        void printActiveUniforms() const;
        void setBool(const string &name, bool value) const;
        void setInt(const string &name, int value) const;
        void setFloat(const string &name, float value) const;
        void setFloatArr(const string &name, const vector<GLfloat> &floats) const;
        void setVec2(const string &name, const glm::vec2 &value) const;
        void setVec2(const string &name, float x, float y) const;
        void setVec3(const string &name, const glm::vec3 &value) const;
        void setVec3(const string &name, float x, float y, float z) const;
        void setVec4(const string &name, const glm::vec4 &value) const;
        void setVec4(const string &name, float x, float y, float z, float w) const;
        void setMat2(const string &name, const glm::mat2 &mat) const;
        void setMat3(const string &name, const glm::mat3 &mat) const;
        void setMat4(const string &name, const glm::mat4 &mat) const;
        void setMat4Array(const string &name, const vector<glm::mat4> &matrices) const;
        bool hasUniform(const string &name) const;

        template<typename T>
        void setUniformArray(const std::string &name, const std::vector<T> &values) const = delete;

        void setUniform(const std::string& name, const UniformValue& value) const;
        [[nodiscard]] GLuint getId() const;
    protected:
        GLuint id;
    };
} // Manager

#endif //SNAKE3_SHADERMANAGER_H
