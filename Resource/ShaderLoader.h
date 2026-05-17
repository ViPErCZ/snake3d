#ifndef SNAKE3_SHADERLOADER_H
#define SNAKE3_SHADERLOADER_H

#include <filesystem>
#include <fstream>
#include <set>
#include <vector>

using namespace std;

namespace fs = std::filesystem;

namespace Resource {
    struct vShader {
        vector<unsigned char> vertex;
    };
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
        static unsigned int loadShader(const fs::path& vertexPath);
        static unsigned int loadShader(const fs::path& vertexPath, const fs::path& fragmentPath);
        static unsigned int loadShader(const fs::path& vertexPath,
                                 const fs::path& geometryPath,
                                 const fs::path& fragmentPath);
        static vShader loadShaderToBuffer(const fs::path& vertexPath);
        static fvShader loadShaderToBuffer(const fs::path& vertexPath, const fs::path& fragmentPath);
        static fgvShader loadShaderToBuffer(const fs::path& vertexPath,
                                                        const fs::path& geometryPath,
                                                        const fs::path& fragmentPath);

        // Načte zdroj a vyresolveuje `#include` direktivy. Bez kompilace.
        // Použití: ShaderRegistry potřebuje string před `injectDefines`.
        static string loadShaderSource(const fs::path& path);

        static unsigned int bindFromBuffer(const string& vertexStr, const string& fragmentStr);
        static unsigned int bindFromBuffer(const string& vertexStr, const string& geometryStr, const string& fragmentStr);
    protected:
        // Backward-compatible wrapper kolem stateful resolveru. Inicializuje
        // file table prázdně, neguarduje proti dvojímu includu globálně mezi
        // sebou-volajícími.
        static void replaceIncludes(const fs::path& base_dir, const string &path, string &source);

        // Rozresolvuje `#include "..."` direktivy v `src`. Stará verze.
        // Necháváme dostupné pro místa která nepotřebují file table.
        static void resolveIncludes(const fs::path& base_dir, std::string& src);

        // Stateful varianta - eviduje již zahrnuté soubory (ochrana před
        // dvojím includem) a buduje `fileTable` pro GLSL `#line` direktivy.
        // `currentFileIndex` je index v `fileTable` pro `src` (== 0 pro root).
        static void resolveIncludesWithState(const fs::path& base_dir,
                                             std::string& src,
                                             std::set<fs::path>& alreadyIncluded,
                                             std::vector<fs::path>& fileTable,
                                             int currentFileIndex);

        static void checkCompileErrors(unsigned int shader, const string &type);
        static unsigned int compileShader(const string &vertexStr);
        static unsigned int compileShader(const string &vertexStr, const string &fragmentStr);
        static unsigned int compileShader(const string &vertexStr, const string &fragmentStr, const string &geometryStr);
        static string readFile(const string &filePath);
    };

} // Resource

#endif //SNAKE3_SHADERLOADER_H
