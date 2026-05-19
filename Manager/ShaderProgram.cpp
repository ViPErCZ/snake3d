#include "ShaderProgram.h"

#include <iostream>

namespace Manager {
    ShaderProgram::ShaderProgram(const GLuint id) : id(id) {
    }

    GLuint ShaderProgram::getId() const {
        return id;
    }

    void ShaderProgram::use() const {
        glUseProgram(id);
    }

    void ShaderProgram::printActiveUniforms() const {
        use();
        GLint uniformCount;
        glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);

        std::cout << "Active uniforms in program " << id << ": " << uniformCount << std::endl;

        GLchar name[256];
        for (GLint i = 0; i < uniformCount; ++i) {
            GLint size;
            GLenum type;
            glGetActiveUniform(id, i, sizeof(name), nullptr, &size, &type, name);
            GLint location = glGetUniformLocation(id, name);
            std::cout << "Uniform #" << i << ": " << name
                      << " | Type: " << type
                      << " | Size: " << size
                      << " | Location: " << location << std::endl;
        }
    }

    void ShaderProgram::setBool(const string &name, bool value) const {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::setInt(const string &name, int value) const {
        // GLint location = glGetUniformLocation(id, name.c_str());
        // if (location == -1) {
        //     std::cerr << "Uniform " << name << " not found in shader\n";
        //     return;
        // }
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::setFloat(const string &name, float value) const {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::setDouble(const string &name, double value) const {
        glUniform1d(glGetUniformLocation(id, name.c_str()), value);
    }

    void ShaderProgram::setFloatArr(const string &name, const vector<GLfloat> &floats) const {
        glUniform1fv(glGetUniformLocation(id, name.c_str()), static_cast<GLsizei>(floats.size()), floats.data());
    }

    void ShaderProgram::setVec2(const string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec2(const string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
    }

    void ShaderProgram::setVec3(const string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec3(const string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    }

    void ShaderProgram::setVec4(const string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void ShaderProgram::setVec4(const string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    }

    void ShaderProgram::setMat2(const string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat3(const string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat4(const string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void ShaderProgram::setMat4Array(const string &name, const vector<glm::mat4> &matrices) const {
        glUniformMatrix4fv(
            glGetUniformLocation(id, name.c_str()),
            static_cast<GLsizei>(matrices.size()),
            GL_FALSE,
            matrices.empty() ? nullptr : reinterpret_cast<const GLfloat *>(matrices.data())
        );
    }

    bool ShaderProgram::hasUniform(const string &name) const {
        return glGetUniformLocation(id, name.c_str()) != -1;
    }

    template<>
    void ShaderProgram::setUniformArray<float>(const std::string &name, const std::vector<float> &values) const {
        const GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) return;

        glUniform1fv(
            location,
            static_cast<GLsizei>(values.size()),
            values.empty() ? nullptr : values.data()
        );
    }

    template<>
    void ShaderProgram::setUniformArray<
        glm::vec2>(const std::string &name, const std::vector<glm::vec2> &values) const {
        const GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) return;

        glUniform2fv(
            location,
            static_cast<GLsizei>(values.size()),
            values.empty() ? nullptr : reinterpret_cast<const GLfloat *>(values.data())
        );
    }

    template<>
    void ShaderProgram::setUniformArray<
        glm::vec3>(const std::string &name, const std::vector<glm::vec3> &values) const {
        const GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) return;

        glUniform3fv(
            location,
            static_cast<GLsizei>(values.size()),
            values.empty() ? nullptr : reinterpret_cast<const GLfloat *>(values.data())
        );
    }

    template<>
    void ShaderProgram::setUniformArray<
        glm::vec4>(const std::string &name, const std::vector<glm::vec4> &values) const {
        const GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) return;

        glUniform4fv(
            location,
            static_cast<GLsizei>(values.size()),
            values.empty() ? nullptr : reinterpret_cast<const GLfloat *>(values.data())
        );
    }

    template<>
    void ShaderProgram::setUniformArray<
        glm::mat4>(const std::string &name, const std::vector<glm::mat4> &values) const {
        const GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) return;

        glUniformMatrix4fv(
            location,
            static_cast<GLsizei>(values.size()),
            GL_FALSE,
            values.empty() ? nullptr : reinterpret_cast<const GLfloat *>(values.data())
        );
    }

    void ShaderProgram::setUniformBlock(const std::string &name, const GLuint blockBinding) const {
        const GLuint blockIndex = glGetUniformBlockIndex(id, name.c_str());
        if (blockIndex != GL_INVALID_INDEX) {
            glUniformBlockBinding(id, blockIndex, blockBinding);
            return;
        }

        std::cerr << "Uniform block index " << name << " not found in shader\n";
    }

    void ShaderProgram::setUniform(const std::string &name, const UniformValue &value) const {
        GLint location = glGetUniformLocation(id, name.c_str());
        if (location == -1) {
            std::cerr << "Uniform " << name << " not found in shader\n";
            return;
        }

        std::visit([&]<typename T0>(T0 &&val) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, bool>) {
                glUniform1i(location, static_cast<int>(val));
            } else if constexpr (std::is_same_v<T, int>) {
                glUniform1i(location, val);
            } else if constexpr (std::is_same_v<T, float>) {
                glUniform1f(location, val);
            } else if constexpr (std::is_same_v<T, std::vector<float> >) {
                glUniform1fv(location, static_cast<GLsizei>(val.size()), val.data());
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
            } else if constexpr (std::is_same_v<T, std::vector<glm::mat4> >) {
                glUniformMatrix4fv(
                    glGetUniformLocation(id, name.c_str()),
                    static_cast<GLsizei>(val.size()),
                    GL_FALSE,
                    val.empty() ? nullptr : reinterpret_cast<const GLfloat *>(val.data())
                );
            }
        }, value);
    }
} // Manager
