#include "ShaderRegistry.h"
#include <iostream>

#include "../Resource/ShaderLoader.h"
#include "../Resource/ShaderPreprocessor.h"

namespace Manager {
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

    std::shared_ptr<ShaderManager> ShaderRegistry::get(const ShaderHandle& handle) {
        const auto it = masters.find(handle.master);
        if (it == masters.end()) {
            std::cerr << "ShaderRegistry::get - unknown master '" << handle.master << "'\n";
            return nullptr;
        }

        const uint64_t key = makeKey(handle.master, handle.features, handle.snippets);
        if (const auto cached = programs.find(key); cached != programs.end()) {
            return cached->second;
        }

        // Features se injektují jako `#define FEATURE_*` a snippets jako
        // text substituce za `// @MARKER` komentáři v master shaderu.
        const Master& m = it->second;
        const auto defines = definesForMask(handle.features);

        GLuint programId = 0;

        if (!m.fragmentPath.has_value()) {
            // Vertex-only (transform feedback). Žádný preprocess - features na
            // particle update shadery nejsou aplikovatelné.
            programId = Resource::ShaderLoader::loadShader(m.vertexPath);
        } else {
            std::string vsSrc = Resource::ShaderLoader::loadShaderSource(m.vertexPath);
            std::string fsSrc = Resource::ShaderLoader::loadShaderSource(*m.fragmentPath);
            vsSrc = Resource::ShaderPreprocessor::injectDefines(vsSrc, defines);
            fsSrc = Resource::ShaderPreprocessor::injectDefines(fsSrc, defines);

            // Aplikuj snippets na vertex i fragment - injectSnippet je no-op
            // pokud marker není přítomen. Snippety mohou samy mít `#include`
            // direktivy, ty se rozresolvují v loadShaderSource.
            for (const auto& [marker, path] : handle.snippets) {
                const std::string snippet = Resource::ShaderLoader::loadShaderSource(path);
                vsSrc = Resource::ShaderPreprocessor::injectSnippet(vsSrc, marker, snippet);
                fsSrc = Resource::ShaderPreprocessor::injectSnippet(fsSrc, marker, snippet);
            }

            if (m.geometryPath.has_value()) {
                std::string gsSrc = Resource::ShaderLoader::loadShaderSource(*m.geometryPath);
                gsSrc = Resource::ShaderPreprocessor::injectDefines(gsSrc, defines);
                for (const auto& [marker, path] : handle.snippets) {
                    const std::string snippet = Resource::ShaderLoader::loadShaderSource(path);
                    gsSrc = Resource::ShaderPreprocessor::injectSnippet(gsSrc, marker, snippet);
                }
                programId = Resource::ShaderLoader::bindFromBuffer(vsSrc, gsSrc, fsSrc);
            } else {
                programId = Resource::ShaderLoader::bindFromBuffer(vsSrc, fsSrc);
            }
        }

        auto program = std::make_shared<ShaderManager>(programId);
        programs.emplace(key, program);
        return program;
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
                                     const std::map<std::string, std::string>& snippets) {
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
        // Snippets - map je seřazená, takže iterace je deterministická.
        for (const auto& [marker, path] : snippets) {
            for (const char c : marker) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                hash *= prime;
            }
            for (const char c : path) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                hash *= prime;
            }
        }
        return hash;
    }
} // Manager
