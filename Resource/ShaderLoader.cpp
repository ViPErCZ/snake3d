#include "ShaderLoader.h"
#include <iostream>
#include <GL/glew.h>

namespace Resource {
    unsigned int ShaderLoader::loadShader(const fs::path &vertexPath, const fs::path &fragmentPath) {
        const auto [fragment, vertex] = loadShaderToBuffer(vertexPath, fragmentPath);
        const string vertexStr(vertex.begin(), vertex.end());
        const string fragmentStr(fragment.begin(), fragment.end());

        return compileShader(vertexStr, fragmentStr);
    }

    unsigned int ShaderLoader::loadShader(const fs::path &vertexPath,
                                          const fs::path &geometryPath,
                                          const fs::path &fragmentPath) {
        const fgvShader shader = loadShaderToBuffer(vertexPath, geometryPath, fragmentPath);
        const string vertexStr(shader.vertex.begin(), shader.vertex.end());
        const string fragmentStr(shader.fragment.begin(), shader.fragment.end());
        const string geomStr(shader.geometry.begin(), shader.geometry.end());

        return compileShader(vertexStr, fragmentStr, geomStr);
    }

    fvShader ShaderLoader::loadShaderToBuffer(const fs::path &vertexPath, const fs::path &fragmentPath) {

        fvShader shader;
        string vertex = readFile(vertexPath);
        string fragment = readFile(fragmentPath);
        replaceIncludes(vertexPath.parent_path(), vertexPath, vertex);
        replaceIncludes(fragmentPath.parent_path(), fragmentPath, fragment);
        shader.vertex.insert(shader.vertex.end(), vertex.begin(), vertex.end());
        shader.fragment.insert(shader.fragment.end(), fragment.begin(), fragment.end());

        return shader;
    }

    fgvShader ShaderLoader::loadShaderToBuffer(const fs::path &vertexPath, const fs::path &geometryPath,
        const fs::path &fragmentPath) {
        fgvShader shader;
        string vertex = readFile(vertexPath);
        string fragment = readFile(fragmentPath);
        string geom = readFile(geometryPath);
        replaceIncludes(vertexPath.parent_path(), vertexPath, vertex);
        replaceIncludes(fragmentPath.parent_path(), fragmentPath, fragment);
        replaceIncludes(geometryPath.parent_path(), geometryPath, geom);
        shader.vertex.insert(shader.vertex.end(), vertex.begin(), vertex.end());
        shader.fragment.insert(shader.fragment.end(), fragment.begin(), fragment.end());
        shader.geometry.insert(shader.geometry.end(), geom.begin(), geom.end());

        return shader;
    }

    unsigned int ShaderLoader::bindFromBuffer(const string &vertexStr, const string &fragmentStr) {
        return compileShader(vertexStr, fragmentStr);
    }

    unsigned int ShaderLoader::bindFromBuffer(
        const string &vertexStr,
        const string &geometryStr,
        const string &fragmentStr)
    {
        return compileShader(vertexStr, fragmentStr, geometryStr);
    }

    void ShaderLoader::checkCompileErrors(const unsigned int shader, const string &type) {
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

    unsigned int ShaderLoader::compileShader(const string &vertexStr, const string &fragmentStr) {
        const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Compile vertex shader
        const char* vertexSource = vertexStr.c_str();
        glShaderSource(vertShader, 1, &vertexSource, nullptr);
        glCompileShader(vertShader);
        checkCompileErrors(vertShader, "VERTEX");

        // Compile fragment shader
        const char* fragmentSource = fragmentStr.c_str();
        glShaderSource(fragShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragShader);
        checkCompileErrors(fragShader, "FRAGMENT");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDetachShader(program, vertShader);
        glDetachShader(program, fragShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);

        return program;
    }

    unsigned int ShaderLoader::compileShader(const string &vertexStr, const string &fragmentStr,
        const string &geometryStr) {
        const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        const GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        const GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER);

        // Compile vertex shader
        const char* vertexSource = vertexStr.c_str();
        glShaderSource(vertShader, 1, &vertexSource, nullptr);
        glCompileShader(vertShader);
        checkCompileErrors(vertShader, "VERTEX");

        // Compile fragment shader
        const char* fragmentSource = fragmentStr.c_str();
        glShaderSource(fragShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragShader);
        checkCompileErrors(fragShader, "FRAGMENT");

        // Compile fragment shader
        const char* geometrySource = geometryStr.c_str();
        glShaderSource(geomShader, 1, &geometrySource, nullptr);
        glCompileShader(geomShader);
        checkCompileErrors(geomShader, "GEOMETRY");

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertShader);
        glAttachShader(program, fragShader);
        glAttachShader(program, geomShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDetachShader(program, vertShader);
        glDetachShader(program, fragShader);
        glDetachShader(program, geomShader);

        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        glDeleteShader(geomShader);

        return program;

    }

    string ShaderLoader::readFile(const string &filePath) {
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

    void ShaderLoader::replaceIncludes(const fs::path &base_dir, const string &path, string &source) {
        try {
            resolveIncludes(base_dir, source);
        } catch (const shader_file_not_found &not_found) {
            throw shader_include_not_found("Failed to resolve include for " + path + ": " + not_found.what());
        }
    }

    void ShaderLoader::resolveIncludes(const fs::path &base_dir, string &src) {
        static constexpr std::string_view include = "#include";

        std::size_t found{};
        while (true) {
            found = src.find(include, found);

            if (found == std::string::npos) {
                return;
            }

            const size_t beg = found + include.length() + 2;
            const size_t end = src.find('"', beg);
            const size_t name_length = end - beg;

            fs::path file_name = src.substr(beg, name_length);

            auto include_src = readFile(base_dir / file_name);

            resolveIncludes(base_dir / file_name.parent_path(), include_src);

            src.replace(found, include.length() + 3 + name_length, include_src);
        }
    }
} // Resource
