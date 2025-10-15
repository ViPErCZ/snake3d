#ifndef SNAKE3_SHADERLOADER_H
#define SNAKE3_SHADERLOADER_H

#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

namespace Resource {
    struct fvShader {
        vector<unsigned char> fragment;
        vector<unsigned char> vertex;
    };
    struct fgvShader {
        vector<unsigned char> fragment;
        vector<unsigned char> geometry;
        vector<unsigned char> vertex;
    };
    class shader_file_not_found final : public std::runtime_error {
    public:
        explicit shader_file_not_found(const std::string& error) : std::runtime_error(error) {}
    };
    class shader_include_not_found final : public std::runtime_error {
    public:
        explicit shader_include_not_found(const std::string& error) : std::runtime_error(error) {}
    };

    class ShaderLoader {
    public:
        static unsigned int loadShader(const fs::path& vertexPath, const fs::path& fragmentPath);
        static unsigned int loadShader(const fs::path& vertexPath,
                                 const fs::path& geometryPath,
                                 const fs::path& fragmentPath);
        static fvShader loadShaderToBuffer(const fs::path& vertexPath, const fs::path& fragmentPath);
        static fgvShader loadShaderToBuffer(const fs::path& vertexPath,
                                                        const fs::path& geometryPath,
                                                        const fs::path& fragmentPath);
        static unsigned int bindFromBuffer(const string& vertexStr, const string& fragmentStr);
        static unsigned int bindFromBuffer(const string& vertexStr, const string& geometryStr, const string& fragmentStr);
    protected:
        static void replaceIncludes(const fs::path& base_dir, const string &path, string &source);
        static void resolveIncludes(const fs::path& base_dir, std::string& src);
        static void checkCompileErrors(unsigned int shader, const string &type);
        static unsigned int compileShader(const string &vertexStr, const string &fragmentStr);
        static unsigned int compileShader(const string &vertexStr, const string &fragmentStr, const string &geometryStr);
        static string readFile(const string &filePath);
    };

} // Resource

#endif //SNAKE3_SHADERLOADER_H
