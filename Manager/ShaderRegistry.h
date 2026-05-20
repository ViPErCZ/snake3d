#ifndef SNAKE3_SHADERREGISTRY_H
#define SNAKE3_SHADERREGISTRY_H

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ShaderFeature.h"
#include "ShaderProgram.h"

namespace fs = std::filesystem;

namespace Manager {
    // Identifikuje konkrétní permutaci shaderu (master + zapnuté features +
    // snippets). Materiály drží `ShaderHandle` místo přímého `ShaderProgram` -
    // registry vrátí program lazy-kompilovaný při prvním requestu.
    //
    // snippets mapuje marker (např. `@MATERIAL_FRAGMENT_POST`) na pořadovou
    // posloupnost cest k .glsl souborům. C2a podporuje víc snippetů na jeden
    // marker - dvě features mohou injektovat na stejné místo, contenty se
    // skládají v pořadí registrace (MaterialBuilder::with). Pořadí + obsah
    // cest jsou součástí cache klíče, takže různé kompozice = různé programy.
    struct ShaderHandle {
        std::string master;
        ShaderFeatureMask features = 0;
        std::map<std::string, std::vector<std::string>> snippets;

        bool operator==(const ShaderHandle& other) const {
            return master == other.master
                && features == other.features
                && snippets == other.snippets;
        }
    };

    // Centrální evidence master shaderů + cache zkompilovaných permutací.
    //
    // V B1 fázi obsluhuje jen `features == 0` cestu - permutace se zapnou až
    // v B2 spolu se `ShaderPreprocessor::injectDefines`. Snippet injekce
    // (B3+) se přidají později.
    class ShaderRegistry {
    public:
        struct Master {
            fs::path vertexPath;
            std::optional<fs::path> geometryPath;
            std::optional<fs::path> fragmentPath; // chybí u transform-feedback shaderů
        };

        void registerMaster(const std::string& name,
                            const fs::path& vertexPath,
                            const fs::path& fragmentPath);

        void registerMaster(const std::string& name,
                            const fs::path& vertexPath,
                            const fs::path& geometryPath,
                            const fs::path& fragmentPath);

        // Vertex-only shader (transform feedback - např. particle update).
        void registerMasterVertexOnly(const std::string& name,
                                      const fs::path& vertexPath);

        // Vrátí program pro daný handle. Při prvním requestu kompiluje a
        // cachuje. Vrací nullptr pokud master není registrován.
        std::shared_ptr<ShaderProgram> get(const ShaderHandle& handle);

        // Smaže cache - vynutí re-kompilaci při dalším `get()`. Užitečné
        // pro hot reload v budoucnu, zatím dostupné jako API hook.
        void clearCache();

        // C4 hot reload: projde cache, pro každý cached program kontroluje
        // mtimes všech watched souborů (master VS/GS/FS + include children +
        // snippety). Když se některý změnil, recompile program s aktuálním
        // obsahem a swapne GL ID přes ShaderProgram::reload - shared_ptr
        // odběratelů zůstává platný. Vrací počet reloaded programů.
        int reloadIfChanged();

        // Iterace přes všechny cached programy. Užívá to např. DepthMapRenderer,
        // který musí broadcastnout shadow-map uniformy (lightSpaceMatrix0/1/2,
        // cascadeEnds, shadowCenter) všem 3D programům - každá permutace
        // (basicShader|features, basicShader|features|HoleMap, ...) je jiné
        // GL program ID. setMat4/setFloat na neexistující uniform je silent
        // no-op, takže můžeme procházet všechny bez ohledu na obsah.
        [[nodiscard]] std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> cachedPrograms() const;

        // Přístup k tabulce masters - debug introspekce.
        [[nodiscard]] bool hasMaster(const std::string& name) const;

    private:
        // FNV-1a 64bit hash pro cache klíč. Stabilní napříč běhy aplikace.
        static uint64_t makeKey(const std::string& master,
                                ShaderFeatureMask features,
                                const std::map<std::string, std::vector<std::string>>& snippets);

        // C4 hot reload entry: udržuje program + handle + seznam paths které
        // přispěly do kompilace, ať reloadIfChanged() ví, co mtime sledovat
        // a co re-kompilovat. mtime se nesnáší v paths samotných (sdílených
        // mezi entries), drží se per-entry snapshot.
        struct CachedProgram {
            std::shared_ptr<ShaderProgram> program;
            ShaderHandle handle;
            std::vector<std::string> watchedPaths;
            std::vector<int64_t> watchedMtimes;
        };

        std::unordered_map<std::string, Master> masters;
        std::unordered_map<uint64_t, CachedProgram> programs;
    };
} // Manager

#endif //SNAKE3_SHADERREGISTRY_H
