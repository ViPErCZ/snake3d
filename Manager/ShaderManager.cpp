#include "ShaderManager.h"

namespace Manager {
    void ShaderManager::loadShader(const string &vertex_path, const string &fragment_path) {
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Read shaders
        string vertShaderStr = readFile(vertex_path);
        string fragShaderStr = readFile(fragment_path);
        const char *vertShaderSrc = vertShaderStr.c_str();
        const char *fragShaderSrc = fragShaderStr.c_str();

        GLint result = GL_FALSE;
        int logLength;

        // Compile vertex shader
        cout << "Compiling vertex shader." << endl;
        glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
        glCompileShader(vertShader);
        checkCompileErrors(vertShader, "VERTEX");

        // Check vertex shader
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);

        // Compile fragment shader
        cout << "Compiling fragment shader." << endl;
        glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
        glCompileShader(fragShader);
        checkCompileErrors(fragShader, "FRAGMENT");

        // Check fragment shader
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);

        cout << "Linking program" << endl;
        GLuint program = glCreateProgram();
        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glGetProgramiv(program, GL_LINK_STATUS, &result);
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        glDetachShader(program, vertShader);
        glDetachShader(program, fragShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        id = program;
    }

    string ShaderManager::readFile(const string &filePath) {
        string content;
        ifstream fileStream(filePath, ios::in);

        if (!fileStream.is_open()) {
            cerr << "Could not read file " << filePath << ". File does not exist." << endl;
            return "";
        }

        string line;
        while (!fileStream.eof()) {
            getline(fileStream, line);
            content.append(line + "\n");
        }

        fileStream.close();

        return content;
    }

    void ShaderManager::checkCompileErrors(GLuint shader, const string &type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
    }

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