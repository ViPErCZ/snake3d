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

        // D4 pre-flight: kompiluje base permutaci (features=0, no snippets) pro
        // každý zaregistrovaný master. Vrátí seznam masterů, kterým compile
        // selhal - App::Init je pak může logovat a buď fatal abort nebo
        // continue (depending on strictness). Lazy-init na první get() runtime
        // by jinak prozradila shader chyby až za N vteřin v gameplay - zde
        // chyba vyplave v Init phase s clear master name.
        struct WarmupResult {
            int compiled = 0;
            std::vector<std::string> failed;
        };
        WarmupResult warmupAll();

        [[nodiscard]] std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> cachedPrograms() const;

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
