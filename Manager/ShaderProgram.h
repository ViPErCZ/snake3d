#ifndef SNAKE3_SHADERPROGRAM_H
#define SNAKE3_SHADERPROGRAM_H

#include "../stdafx.h"
#include <string>
#include <unordered_map>
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
    glm::mat4,
    vector<float>,
    vector<glm::mat4>
>;

namespace Manager {

    class ShaderProgram {
    public:
        explicit ShaderProgram(GLuint id);

        // C4: hot reload - swapne interní GL program ID. Smaže starý handle,
        // aby předchozí program nevisel jako orphan. shared_ptr<ShaderProgram>
        // zůstává validní (stejný objekt), takže materiály držící pointer
        // dostanou nový program bez re-acquisition přes registry.
        void reload(GLuint newId);

        void use() const;
        void printActiveUniforms() const;
        void setBool(const string &name, bool value) const;
        void setInt(const string &name, int value) const;
        void setFloat(const string &name, float value) const;
        void setDouble(const string &name, double value) const;
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
        [[nodiscard]] bool hasUniform(const string &name) const;

        template<typename T>
        void setUniformArray(const std::string &name, const std::vector<T> &values) const = delete;

        void setUniformBlock(const std::string &name, GLuint blockBinding) const;

        void setUniform(const std::string& name, const UniformValue& value) const;
        [[nodiscard]] GLuint getId() const;
    protected:
        GLuint id;

        // Cache pro glGetUniformLocation - location se nemění po linkování
        // programu, takže lze cachovat. Per-frame uniform spam (611 draws ×
        // ~30 setterů) bez cache = ~18 000 GL syscalls per frame; s cache
        // jen N unique names × 1× lookup za celý běh.
        // -1 v cache = uniform neexistuje (optimized away by compiler).
        // mutable kvůli const setterům (logická const, fyzická lazy init).
        mutable std::unordered_map<std::string, GLint> locationCache;
        GLint getUniformLocation(const std::string& name) const;
    };
} // Manager

#endif //SNAKE3_SHADERPROGRAM_H
