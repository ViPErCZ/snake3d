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

    class ShaderLoader {
    public:
        static unsigned int loadShader(const fs::path& vertexPath, const fs::path& fragmentPath);
    protected:
        static void checkCompileErrors(unsigned int shader, const string &type);
        static string readFile(const string &filePath);
    };

} // Resource

#endif //SNAKE3_SHADERLOADER_H
