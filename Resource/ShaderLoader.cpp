#include "ShaderLoader.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <GL/glew.h>

namespace Resource {
    unsigned int ShaderLoader::loadShader(const fs::path &vertexPath) {
        const auto vertex = loadShaderToBuffer(vertexPath);
        const string vertexStr(vertex.vertex.begin(), vertex.vertex.end());

        return compileShader(vertexStr);
    }

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

    vShader ShaderLoader::loadShaderToBuffer(const fs::path &vertexPath) {
        vShader shader;
        string vertex = readFile(vertexPath);
        replaceIncludes(vertexPath.parent_path(), vertexPath, vertex);
        shader.vertex.insert(shader.vertex.end(), vertex.begin(), vertex.end());

        return shader;
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

    unsigned int ShaderLoader::compileShader(const string &vertexStr) {
        const GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

        // Compile vertex shader
        const char* vertexSource = vertexStr.c_str();
        glShaderSource(vertShader, 1, &vertexSource, nullptr);
        glCompileShader(vertShader);
        checkCompileErrors(vertShader, "VERTEX");

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertShader);

        // Transform Feedback pro stav částic (interleaved do jednoho VBO)
        const char* varyings[] = {
            "outPos",
            "outVel",
            "outLife",
            "outSeed"
        };

        glTransformFeedbackVaryings(program, 4, varyings, GL_INTERLEAVED_ATTRIBS);
        glLinkProgram(program);

        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDetachShader(program, vertShader);
        glDeleteShader(vertShader);

        return program;
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

        const GLuint program = glCreateProgram();
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
            std::set<fs::path> alreadyIncluded;
            std::vector<fs::path> fileTable;
            // Root file zaregistrujeme jako index 0. fs::canonical() by byla
            // přesnější, ale soubor nemusí v této fázi vždy existovat na disku
            // (testovací paths atd.) - držíme se prosté path porovnávky.
            alreadyIncluded.insert(path);
            fileTable.emplace_back(path);

            resolveIncludesWithState(base_dir, source, alreadyIncluded, fileTable, 0);
        } catch (const shader_file_not_found &not_found) {
            throw shader_include_not_found("Failed to resolve include for " + path + ": " + not_found.what());
        }
    }

    void ShaderLoader::resolveIncludes(const fs::path &base_dir, string &src) {
        // Bezstavová varianta - inicializuje state na požádání. Necháváme
        // dostupné pro místa která filetable nevyužijí (např. tests).
        std::set<fs::path> alreadyIncluded;
        std::vector<fs::path> fileTable;
        resolveIncludesWithState(base_dir, src, alreadyIncluded, fileTable, 0);
    }

    void ShaderLoader::resolveIncludesWithState(const fs::path &base_dir,
                                                std::string &src,
                                                std::set<fs::path> &alreadyIncluded,
                                                std::vector<fs::path> &fileTable,
                                                const int currentFileIndex) {
        static constexpr std::string_view include = "#include";

        std::size_t found = 0;
        while (true) {
            found = src.find(include, found);

            if (found == std::string::npos) {
                return;
            }

            const size_t beg = found + include.length() + 2; // skip ' "'
            const size_t end = src.find('"', beg);
            if (end == std::string::npos) {
                // Malformed include - opustíme smyčku, kompilátor zařve.
                return;
            }
            const size_t name_length = end - beg;
            const fs::path file_name = src.substr(beg, name_length);
            const fs::path full_path = base_dir / file_name;

            // Délka řádky s `#include` (po včetně '\n' pokud existuje), abychom
            // ho mohli vyhodit ze zdroje a nahradit injektovanou expanzí.
            const size_t after_quote = end + 1;
            const size_t newline = src.find('\n', after_quote);
            const size_t replace_len = (newline == std::string::npos)
                                       ? src.size() - found
                                       : newline - found + 1;

            // Číslo řádku v src kde stál #include - potřebujeme pro `#line`
            // marker za expandovaným blokem aby šly chyby v rodičovském
            // souboru číst správně.
            const int includeLine = static_cast<int>(
                std::count(src.begin(), src.begin() + found, '\n')
            ) + 1;

            std::string injection;
            if (alreadyIncluded.contains(full_path)) {
                // Duplicitní include - nahradíme jen komentářem aby řádky
                // sedaly. Ochrana je důležitá pro GLSL který nepodporuje
                // `#pragma once` a redeklarace symbolu = chyba.
                injection = "// (skipped duplicate include: " + file_name.string() + ")\n";
            } else {
                alreadyIncluded.insert(full_path);
                const int includedFileIdx = static_cast<int>(fileTable.size());
                fileTable.emplace_back(full_path);

                std::string include_src = readFile(full_path);
                resolveIncludesWithState(full_path.parent_path(), include_src,
                                         alreadyIncluded, fileTable, includedFileIdx);

                // GLSL 330+: `#line <line> <file-int>`. File je číslo, ne
                // string - mapování čísel na cesty je v `fileTable`.
                std::ostringstream out;
                out << "#line 1 " << includedFileIdx << "\n";
                out << include_src;
                if (!include_src.empty() && include_src.back() != '\n') {
                    out << '\n';
                }
                // Po expanzi vrátíme line tracking zpět na rodiče.
                out << "#line " << (includeLine + 1) << " " << currentFileIndex << "\n";
                injection = out.str();
            }

            src.replace(found, replace_len, injection);
            found += injection.size();
        }
    }
} // Resource
