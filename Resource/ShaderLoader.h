#ifndef SNAKE3_SHADERLOADER_H
#define SNAKE3_SHADERLOADER_H

#include "../stdafx.h"
#include <filesystem>
#include <memory>
#include <iostream>
#include <fstream>

using namespace std;

namespace fs = std::filesystem;

namespace Resource {
    class shader_file_not_found : public std::runtime_error {
    public:
        explicit shader_file_not_found(const std::string& error) : std::runtime_error(error) {}
    };
    class shader_include_not_found : public std::runtime_error {
    public:
        explicit shader_include_not_found(const std::string& error) : std::runtime_error(error) {}
    };

    class ShaderLoader {
    public:
        static unsigned int loadShader(const fs::path& vertexPath, const fs::path& fragmentPath);
    protected:
        static void replaceIncludes(const fs::path& base_dir, const string &path, string &source);
        static void resolveIncludes(const fs::path& base_dir, std::string& src);
        static void checkCompileErrors(unsigned int shader, const string &type);
        static string readFile(const string &filePath);
    };

} // Resource

#endif //SNAKE3_SHADERLOADER_H
