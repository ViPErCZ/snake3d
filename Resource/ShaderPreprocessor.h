#ifndef SNAKE3_SHADERPREPROCESSOR_H
#define SNAKE3_SHADERPREPROCESSOR_H

#include <string>
#include <vector>

namespace Resource {
    // Statická pomocná třída pro úpravu GLSL zdroje před kompilací.
    //
    // Volá se po `ShaderLoader::resolveIncludes` (tj. nad zdrojem který už má
    // expandované `#include` direktivy) a před `glShaderSource`.
    //
    // Aktuální schopnosti (B1 fáze):
    //   - injectDefines: vloží `#define X`/`#define X Y` za `#version` line
    //
    // Plánované (B3+):
    //   - injectSnippet: nahradí placeholder `<MATERIAL_FRAGMENT_PRE>` obsahem
    //     externího `.glsl` souboru (snippets z files, ne string literals).
    class ShaderPreprocessor {
    public:
        // Vloží `#define <name>` (bez hodnoty) za `#version` direktivu. Pokud
        // zdroj `#version` nemá, vloží je na začátek.
        static std::string injectDefines(const std::string& source,
                                         const std::vector<std::string>& defines);

        // Stejné jako výše, ale s hodnotou (#define name value).
        static std::string injectDefinesKv(const std::string& source,
                                           const std::vector<std::pair<std::string, std::string>>& defines);

        // Najde řádek obsahující `marker` (typicky `// @MATERIAL_FRAGMENT_PRE`)
        // a nahradí ho obsahem `snippet`. Marker se hledá jako substring -
        // řádek může mít odsazení nebo doprovodné komentáře, jen musí marker
        // obsahovat. Pokud marker nenajde, vrací zdroj beze změny (no-op);
        // pokud `snippet` je prázdný, taktéž no-op (efektivně ponechá marker
        // řádek). Nahrazuje pouze první výskyt - další ponechá jako-jsou.
        static std::string injectSnippet(const std::string& source,
                                         const std::string& marker,
                                         const std::string& snippet);
    };
} // Resource

#endif //SNAKE3_SHADERPREPROCESSOR_H
