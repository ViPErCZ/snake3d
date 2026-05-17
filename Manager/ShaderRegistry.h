#ifndef SNAKE3_SHADERREGISTRY_H
#define SNAKE3_SHADERREGISTRY_H

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "ShaderFeature.h"
#include "ShaderManager.h"

namespace fs = std::filesystem;

namespace Manager {
    // Identifikuje konkrétní permutaci shaderu (master + zapnuté features).
    // Materiály drží `ShaderHandle` místo přímého `ShaderManager` - registry
    // vrátí program lazy-kompilovaný při prvním requestu.
    struct ShaderHandle {
        std::string master;
        ShaderFeatureMask features = 0;

        bool operator==(const ShaderHandle& other) const {
            return master == other.master && features == other.features;
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
        std::shared_ptr<ShaderManager> get(const ShaderHandle& handle);

        // Smaže cache - vynutí re-kompilaci při dalším `get()`. Užitečné
        // pro hot reload v budoucnu, zatím dostupné jako API hook.
        void clearCache();

        // Přístup k tabulce masters - debug introspekce.
        [[nodiscard]] bool hasMaster(const std::string& name) const;

    private:
        // FNV-1a 64bit hash pro cache klíč. Stabilní napříč běhy aplikace.
        static uint64_t makeKey(const std::string& master, ShaderFeatureMask features);

        std::unordered_map<std::string, Master> masters;
        std::unordered_map<uint64_t, std::shared_ptr<ShaderManager>> programs;
    };
} // Manager

#endif //SNAKE3_SHADERREGISTRY_H
