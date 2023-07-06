#include "ShaderLoader.h"

namespace Resource {
    unsigned int ShaderLoader::loadShader(const fs::path &vertexPath, const fs::path &fragmentPath) {
        GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        // Read shaders
        string vertShaderStr = readFile(vertexPath);
        string fragShaderStr = readFile(fragmentPath);
        cout << fragmentPath.parent_path() << endl;
        replaceIncludes(fragmentPath.parent_path(), fragmentPath, fragShaderStr);
        replaceIncludes(vertexPath.parent_path(), vertexPath, vertShaderStr);
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

        return program;
    }

    void ShaderLoader::checkCompileErrors(unsigned int shader, const string &type) {
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
        } catch (const shader_file_not_found& not_found) {
            throw shader_include_not_found("Failed to resolve include for " + path + ": " + not_found.what());
        }
    }

    void ShaderLoader::resolveIncludes(const fs::path &base_dir, string &src) {
        static constexpr std::string_view include = "#include";

        std::size_t found {};
        while (true) {
            found = src.find(include, found);

            if (found == std::string::npos) {
                return;
            }

            size_t beg = found + include.length() + 2;
            size_t end = src.find('"', beg);
            size_t name_length = end - beg;

            fs::path file_name = src.substr(beg, name_length);

            auto include_src = readFile(base_dir / file_name);

            resolveIncludes(base_dir / file_name.parent_path(), include_src);

            src.replace(found, include.length() + 3 + name_length, include_src);
        }
    }

} // Resource