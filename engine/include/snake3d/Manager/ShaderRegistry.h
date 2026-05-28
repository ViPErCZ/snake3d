#ifndef SNAKE3_SHADERREGISTRY_H
#define SNAKE3_SHADERREGISTRY_H

#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <snake3d/Manager/ShaderFeature.h>
#include <snake3d/Manager/ShaderProgram.h>

namespace fs = std::filesystem;

namespace Manager {
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

    class ShaderRegistry {
    public:
        struct Master {
            fs::path vertexPath;
            std::optional<fs::path> geometryPath;
            std::optional<fs::path> fragmentPath;
        };

        void registerMaster(const std::string& name,
                            const fs::path& vertexPath,
                            const fs::path& fragmentPath);

        void registerMaster(const std::string& name,
                            const fs::path& vertexPath,
                            const fs::path& geometryPath,
                            const fs::path& fragmentPath);

        void registerMasterVertexOnly(const std::string& name,
                                      const fs::path& vertexPath);

        std::shared_ptr<ShaderProgram> get(const ShaderHandle& handle);

        void clearCache();

        void reloadIfChanged();

        struct WarmupResult {
            int compiled = 0;
            std::vector<std::string> failed;
        };
        WarmupResult warmupAll();

        [[nodiscard]] std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> cachedPrograms() const;

        // F1 iter 3 ImGui inspector: snapshot per-program metadata pro debug
        // UI. Vrací plain data, ne pointery na interní CachedProgram - tester
        // (ImGui) je decoupled od implementace.
        struct ProgramInfo {
            uint64_t key;          // cache key (FNV hash master+features+snippets)
            unsigned int glId;     // GL program handle (po hot reload se mění)
            std::string master;    // master shader name (např. "basicShader")
            ShaderFeatureMask features;
            std::vector<std::string> watchedPaths;  // master + includes + snippets
            size_t snippetCount;
        };
        [[nodiscard]] std::vector<ProgramInfo> getCachedProgramInfo() const;

        // Force recompile konkrétního programu (Inspector "Reload" button).
        // Pokud key není v cache, no-op + log. Compile chyba: log, starý
        // program zůstává.
        void reloadProgram(uint64_t key);

        [[nodiscard]] bool hasMaster(const std::string& name) const;

    private:
        static uint64_t makeKey(const std::string& master,
                                ShaderFeatureMask features,
                                const std::map<std::string, std::vector<std::string>>& snippets);

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
