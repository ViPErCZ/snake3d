#include "ShaderManager.h"

namespace Manager {

    ShaderManager::ShaderManager(GLuint id) : id(id) {}

    void ShaderManager::use() const {
        glUseProgram(id);
    }

    void ShaderManager::setBool(const string &name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int) value);
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

} // Manager