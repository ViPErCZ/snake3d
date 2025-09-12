#include "ShaderManager.h"

#include <iostream>

namespace Manager {

    ShaderManager::ShaderManager(GLuint id) : id(id) {}

    void ShaderManager::use() const {
        glUseProgram(id);
    }

    void ShaderManager::setBool(const string &name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderManager::setInt(const string &name, int value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderManager::setFloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderManager::setVec2(const string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderManager::setVec2(const string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
    }

    void ShaderManager::setVec3(const string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderManager::setVec3(const string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    }

    void ShaderManager::setVec4(const string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderManager::setVec4(const string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    }

    void ShaderManager::setMat2(const string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderManager::setMat3(const string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderManager::setMat4(const string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    GLuint ShaderManager::getId() const {
        return id;
    }

    void ShaderManager::setUniform(const std::string& name, const UniformValue& value) const {
        GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) {
            std::cerr << "Uniform " << name << " not found in shader\n";
            return;
        }

        std::visit([&]<typename T0>(T0&& val) {
           using T = std::decay_t<T0>;
           if constexpr (std::is_same_v<T, bool>) {
               glUniform1i(location, static_cast<int>(val));
           } else if constexpr (std::is_same_v<T, int>) {
               glUniform1i(location, val);
            } else if constexpr (std::is_same_v<T, float>) {
                glUniform1f(location, val);
            } else if constexpr (std::is_same_v<T, glm::vec2>) {
                glUniform2fv(location, 1, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                glUniform3fv(location, 1, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                glUniform4fv(location, 1, &val[0]);
            } else if constexpr (std::is_same_v<T, glm::mat2>) {
                glUniformMatrix2fv(location, 1, GL_FALSE, &val[0][0]);
            } else if constexpr (std::is_same_v<T, glm::mat3>) {
                glUniformMatrix3fv(location, 1, GL_FALSE, &val[0][0]);
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
            }
        }, value);
    }
} // Manager