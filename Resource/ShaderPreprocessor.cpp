#include "ShaderPreprocessor.h"

#include <sstream>

namespace Resource {
    namespace {
        // Najde konec `#version ...` line včetně případného `core` profilu.
        // Vrátí pozici za '\n' za version line, nebo std::string::npos pokud
        // zdroj žádný `#version` nemá.
        size_t findInsertionPoint(const std::string& source) {
            const auto versionPos = source.find("#version");
            if (versionPos == std::string::npos) {
                return std::string::npos;
            }
            const auto newlinePos = source.find('\n', versionPos);
            if (newlinePos == std::string::npos) {
                return source.size();
            }
            return newlinePos + 1;
        }
    }

    std::string ShaderPreprocessor::injectDefines(const std::string& source,
                                                  const std::vector<std::string>& defines) {
        if (defines.empty()) {
            return source;
        }

        std::ostringstream injection;
        for (const auto& name : defines) {
            injection << "#define " << name << "\n";
        }

        const auto insertAt = findInsertionPoint(source);
        if (insertAt == std::string::npos) {
            // Bezpečný fallback - shader bez `#version` (např. fragment include
            // glsl). Vlož na začátek; GLSL kompilátor sám zařve pokud něco vadí.
            return injection.str() + source;
        }

        std::string result;
        result.reserve(source.size() + injection.tellp());
        result.append(source, 0, insertAt);
        result.append(injection.str());
        result.append(source, insertAt, std::string::npos);
        return result;
    }

    std::string ShaderPreprocessor::injectDefinesKv(const std::string& source,
                                                    const std::vector<std::pair<std::string, std::string>>& defines) {
        if (defines.empty()) {
            return source;
        }

        std::ostringstream injection;
        for (const auto& [name, value] : defines) {
            injection << "#define " << name;
            if (!value.empty()) {
                injection << ' ' << value;
            }
            injection << '\n';
        }

        const auto insertAt = findInsertionPoint(source);
        if (insertAt == std::string::npos) {
            return injection.str() + source;
        }

        std::string result;
        result.reserve(source.size() + injection.tellp());
        result.append(source, 0, insertAt);
        result.append(injection.str());
        result.append(source, insertAt, std::string::npos);
        return result;
    }
} // Resource
