#include "ShaderRegistry.h"

#include <filesystem>
#include <iostream>
#include <ranges>
#include <system_error>

#include "../Resource/ShaderLoader.h"
#include "../Resource/ShaderPreprocessor.h"

namespace Manager {
    namespace {
        int64_t fileMtime(const std::string& path) {
            std::error_code ec;
            const auto t = std::filesystem::last_write_time(path, ec);
            if (ec) return 0;
            return t.time_since_epoch().count();
        }
    }

    struct CompileResult {
        GLuint programId;
        std::vector<std::string> watchedPaths;
    };

    static CompileResult compileForHandle(const ShaderRegistry::Master& m,
                                          const ShaderHandle& handle) {
        const auto defines = definesForMask(handle.features);
        std::vector<fs::path> includedFiles;
        GLuint programId = 0;

        if (!m.fragmentPath.has_value()) {
            // Vertex-only (transform feedback). Žádný preprocess - features na
            // particle update shadery nejsou aplikovatelné.
            includedFiles.push_back(m.vertexPath);
            programId = Resource::ShaderLoader::loadShader(m.vertexPath);
        } else {
            std::string vsSrc = Resource::ShaderLoader::loadShaderSource(m.vertexPath, includedFiles);
            std::string fsSrc = Resource::ShaderLoader::loadShaderSource(*m.fragmentPath, includedFiles);
            std::optional<std::string> gsSrc;
            if (m.geometryPath.has_value()) {
                gsSrc = Resource::ShaderLoader::loadShaderSource(*m.geometryPath, includedFiles);
            }

            vsSrc = Resource::ShaderPreprocessor::injectDefines(vsSrc, defines);
            fsSrc = Resource::ShaderPreprocessor::injectDefines(fsSrc, defines);
            if (gsSrc.has_value()) {
                gsSrc = Resource::ShaderPreprocessor::injectDefines(*gsSrc, defines);
            }

            const auto vsMarkers = Resource::ShaderPreprocessor::scanMaterialMarkers(vsSrc);
            const auto fsMarkers = Resource::ShaderPreprocessor::scanMaterialMarkers(fsSrc);
            std::vector<Resource::MarkerLocation> gsMarkers;
            if (gsSrc.has_value()) {
                gsMarkers = Resource::ShaderPreprocessor::scanMaterialMarkers(*gsSrc);
            }

            Resource::ShaderPreprocessor::validateUniqueMarkers(vsMarkers, m.vertexPath.string());
            Resource::ShaderPreprocessor::validateUniqueMarkers(fsMarkers, m.fragmentPath->string());
            Resource::ShaderPreprocessor::warnUnknownMarkers(vsMarkers, m.vertexPath.string());
            Resource::ShaderPreprocessor::warnUnknownMarkers(fsMarkers, m.fragmentPath->string());
            if (gsSrc.has_value()) {
                Resource::ShaderPreprocessor::validateUniqueMarkers(gsMarkers, m.geometryPath->string());
                Resource::ShaderPreprocessor::warnUnknownMarkers(gsMarkers, m.geometryPath->string());
            }

            std::map<std::string, std::string> snippetTexts;
            for (const auto& [marker, paths] : handle.snippets) {
                std::string combined;
                for (const auto& path : paths) {
                    if (!combined.empty() && combined.back() != '\n') {
                        combined.push_back('\n');
                    }
                    combined += Resource::ShaderLoader::loadShaderSource(path, includedFiles);
                }
                snippetTexts.emplace(marker, std::move(combined));
            }

            std::set<std::string> presentMarkers;
            for (const auto& mk : vsMarkers) presentMarkers.insert(mk.name);
            for (const auto& mk : fsMarkers) presentMarkers.insert(mk.name);
            for (const auto& mk : gsMarkers) presentMarkers.insert(mk.name);
            Resource::ShaderPreprocessor::validateSnippetsMatch(snippetTexts, presentMarkers);

            vsSrc = Resource::ShaderPreprocessor::applySnippets(vsSrc, snippetTexts);
            fsSrc = Resource::ShaderPreprocessor::applySnippets(fsSrc, snippetTexts);
            if (gsSrc.has_value()) {
                gsSrc = Resource::ShaderPreprocessor::applySnippets(*gsSrc, snippetTexts);
                programId = Resource::ShaderLoader::bindFromBuffer(vsSrc, *gsSrc, fsSrc);
            } else {
                programId = Resource::ShaderLoader::bindFromBuffer(vsSrc, fsSrc);
            }
        }

        std::vector<std::string> watched;
        watched.reserve(includedFiles.size());
        for (const auto& f : includedFiles) {
            watched.push_back(f.string());
        }
        return {programId, std::move(watched)};
    }

    void ShaderRegistry::registerMaster(const std::string& name,
                                        const fs::path& vertexPath,
                                        const fs::path& fragmentPath) {
        masters[name] = Master{vertexPath, std::nullopt, fragmentPath};
    }

    void ShaderRegistry::registerMaster(const std::string& name,
                                        const fs::path& vertexPath,
                                        const fs::path& geometryPath,
                                        const fs::path& fragmentPath) {
        masters[name] = Master{vertexPath, geometryPath, fragmentPath};
    }

    void ShaderRegistry::registerMasterVertexOnly(const std::string& name,
                                                  const fs::path& vertexPath) {
        masters[name] = Master{vertexPath, std::nullopt, std::nullopt};
    }

    std::shared_ptr<ShaderProgram> ShaderRegistry::get(const ShaderHandle& handle) {
        const auto it = masters.find(handle.master);
        if (it == masters.end()) {
            std::cerr << "ShaderRegistry::get - unknown master '" << handle.master << "'\n";
            return nullptr;
        }

        const uint64_t key = makeKey(handle.master, handle.features, handle.snippets);
        if (const auto cached = programs.find(key); cached != programs.end()) {
            return cached->second.program;
        }

        auto result = compileForHandle(it->second, handle);
        auto program = std::make_shared<ShaderProgram>(result.programId);

        std::vector<int64_t> mtimes;
        mtimes.reserve(result.watchedPaths.size());
        for (const auto& p : result.watchedPaths) {
            mtimes.push_back(fileMtime(p));
        }

        programs.emplace(key, CachedProgram{
            program,
            handle,
            std::move(result.watchedPaths),
            std::move(mtimes),
        });
        return program;
    }

    ShaderRegistry::WarmupResult ShaderRegistry::warmupAll() {
        WarmupResult result;
        for (const auto& [name, master] : masters) {
            try {
                const auto program = get({name, 0, {}});
                if (program && program->getId() != 0) {
                    ++result.compiled;
                } else {
                    result.failed.push_back(name);
                }
            } catch (const std::exception& e) {
                // ShaderLoader compile errors throw runtime_error. Lift error
                // do log + result.failed; volající rozhodne o fatal abort.
                std::cerr << "[ShaderRegistry] warmup failed for '" << name
                          << "': " << e.what() << "\n";
                result.failed.push_back(name);
            }
        }
        return result;
    }

    void ShaderRegistry::reloadIfChanged() {
        int reloaded = 0;
        for (auto &entry: programs | views::values) {
            bool changed = false;
            for (size_t i = 0; i < entry.watchedPaths.size(); ++i) {
                if (fileMtime(entry.watchedPaths[i]) != entry.watchedMtimes[i]) {
                    changed = true;
                    break;
                }
            }
            if (!changed) continue;

            const auto masterIt = masters.find(entry.handle.master);
            if (masterIt == masters.end()) continue;

            try {
                auto result = compileForHandle(masterIt->second, entry.handle);
                entry.program->reload(result.programId);
                entry.watchedPaths = std::move(result.watchedPaths);
                entry.watchedMtimes.clear();
                entry.watchedMtimes.reserve(entry.watchedPaths.size());
                for (const auto& p : entry.watchedPaths) {
                    entry.watchedMtimes.push_back(fileMtime(p));
                }
                ++reloaded;
            } catch (const std::exception& e) {
                // Kompilace/validace selhala - starý program zůstává funkční,
                // log error a pokračuj. Uživatel uvidí v konzoli důvod a může
                // opravit shader bez restartu hry.
                std::cerr << "[ShaderRegistry] reload failed for '" << entry.handle.master
                          << "': " << e.what() << "\n";
            }
        }
        if (reloaded > 0) {
            std::cout << "[ShaderRegistry] hot-reloaded " << reloaded << " program(s)\n";
        }
    }

    std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> ShaderRegistry::cachedPrograms() const {
        std::unordered_map<uint64_t, std::shared_ptr<ShaderProgram>> out;
        out.reserve(programs.size());
        for (const auto& [k, entry] : programs) {
            out.emplace(k, entry.program);
        }
        return out;
    }

    void ShaderRegistry::clearCache() {
        // Pozor: program objekty drží GL handles. shared_ptr je destruktuje
        // až refcount klesne na 0, takže pokud někdo materiál ještě drží,
        // jeho GL program zůstane funkční. To je žádoucí pro hot reload -
        // staré objekty doběhnou, nové requesty kompilují fresh.
        programs.clear();
    }

    bool ShaderRegistry::hasMaster(const std::string& name) const {
        return masters.contains(name);
    }

    uint64_t ShaderRegistry::makeKey(const std::string& master,
                                     const ShaderFeatureMask features,
                                     const std::map<std::string, std::vector<std::string>>& snippets) {
        // FNV-1a 64bit. Stačí, kolize jsou v praxi nulové při <1000 entries.
        constexpr uint64_t prime  = 1099511628211ull;
        constexpr uint64_t offset = 14695981039346656037ull;

        uint64_t hash = offset;
        for (const char c : master) {
            hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
            hash *= prime;
        }
        // Feature mask přimícháme po stringu.
        for (int byte = 0; byte < 4; ++byte) {
            hash ^= (features >> (byte * 8)) & 0xFFu;
            hash *= prime;
        }
        // Snippets - map je seřazená, vector pořadí významné (pořadí registrace
        // v MaterialBuilder určuje concatenaci kódu, dvě stejné cesty v jiném
        // pořadí = jiný shader).
        for (const auto& [marker, paths] : snippets) {
            for (const char c : marker) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                hash *= prime;
            }
            for (const auto& path : paths) {
                for (const char c : path) {
                    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                    hash *= prime;
                }
                // Separátor mezi paths v rámci stejného markeru, aby
                // {"a", "bc"} a {"ab", "c"} dávaly různé hashe.
                hash ^= 0xFFu;
                hash *= prime;
            }
        }
        return hash;
    }
} // Manager
