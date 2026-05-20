#ifndef SNAKE3_SHADERPREPROCESSOR_H
#define SNAKE3_SHADERPREPROCESSOR_H

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace Resource {
    // Vyhozeno z `processMaterialPlaceholders`/aggregate validation při neshodě
    // mezi master shaderem a feature snippety. Distinct exception, aby tests
    // mohly catchnout cíleně bez catchování ostatních runtime_error.
    class shader_placeholder_error final : public std::runtime_error {
    public:
        explicit shader_placeholder_error(const std::string& msg) : std::runtime_error(msg) {}
    };

    // Marker pozorovaný v master shader source: jméno (`@MATERIAL_*`) + line.
    struct MarkerLocation {
        std::string name;
        int line;
    };

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
        //
        // Pozn.: Jednoduchý replace bez validace. Pro orchestrovaný flow
        // (validace duplicit, mismatch, whitelist) viz processMaterialPlaceholders.
        static std::string injectSnippet(const std::string& source,
                                         const std::string& marker,
                                         const std::string& snippet);

        // Posbírá všechny `@MATERIAL_*` markery v source spolu s line numbers
        // (1-based). Token se čte jako `@` + identifikátor (alfanum + '_').
        // Markery které neměly prefix `@MATERIAL_` jsou ignorovány (např.
        // GLSL attribute syntax).
        static std::vector<MarkerLocation> scanMaterialMarkers(const std::string& source);

        // Throws shader_placeholder_error pokud stejný marker je v `markers`
        // víckrát. `sourceLabel` se objeví v chybové hlášce.
        static void validateUniqueMarkers(const std::vector<MarkerLocation>& markers,
                                          const std::string& sourceLabel);

        // Stderr warning pro markery v `markers` které nejsou ve whitelistu
        // (Manager::kKnownMaterialPlaceholders). Indikuje překlep v master
        // shaderu nebo zapomenutý update whitelistu.
        static void warnUnknownMarkers(const std::vector<MarkerLocation>& markers,
                                       const std::string& sourceLabel);

        // Throws shader_placeholder_error pro každý snippet jehož marker není
        // v `presentMarkers` (= feature deklaruje placeholder, který v master
        // shaderu chybí). To je typicky bug ve feature::snippetPaths().
        static void validateSnippetsMatch(const std::map<std::string, std::string>& snippets,
                                          const std::set<std::string>& presentMarkers);

        // Nahradí marker řádky obsahem `snippets[marker]`. Markery v source
        // bez záznamu ve `snippets` jsou ponechány jako-jsou (no-op default,
        // řádek se zachová jako neškodný komentář). Bez validace - tu spustí
        // volající přes scan + validate*.
        static std::string applySnippets(const std::string& source,
                                         const std::map<std::string, std::string>& snippets);

        // Convenience: scan + validate unique + warn unknown + validate
        // snippets-match + applySnippets v jednom kroku. Throw při duplicitě
        // nebo mismatch. Pro multi-stage flow (VS+FS+GS) viz ShaderRegistry,
        // který orchestrovuje validaci napříč všemi stage zdroji.
        static std::string processMaterialPlaceholders(
            const std::string& source,
            const std::string& sourceLabel,
            const std::map<std::string, std::string>& snippets);
    };
} // Resource

#endif //SNAKE3_SHADERPREPROCESSOR_H
