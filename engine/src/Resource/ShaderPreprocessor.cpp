#include <snake3d/Resource/ShaderPreprocessor.h>

#include <cctype>
#include <iostream>
#include <sstream>

#include <snake3d/Manager/MaterialPlaceholder.h>

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

    std::string ShaderPreprocessor::injectSnippet(const std::string& source,
                                                  const std::string& marker,
                                                  const std::string& snippet) {
        if (marker.empty() || snippet.empty()) {
            return source;
        }

        const auto markerPos = source.find(marker);
        if (markerPos == std::string::npos) {
            return source;
        }

        // Najdi začátek řádku obsahujícího marker (znak za posledním '\n'
        // před marker pozicí, případně začátek zdroje).
        const auto lineStart = (markerPos == 0)
            ? 0
            : source.rfind('\n', markerPos - 1);
        const size_t replaceFrom = (lineStart == std::string::npos) ? 0 : lineStart + 1;

        // Konec řádku - znak za '\n' za marker pozicí (nebo konec zdroje).
        const auto newlinePos = source.find('\n', markerPos);
        const size_t replaceTo = (newlinePos == std::string::npos)
            ? source.size()
            : newlinePos + 1;

        std::string result;
        result.reserve(source.size() - (replaceTo - replaceFrom) + snippet.size() + 1);
        result.append(source, 0, replaceFrom);
        result.append(snippet);
        if (!snippet.empty() && snippet.back() != '\n') {
            result.push_back('\n');
        }
        result.append(source, replaceTo, std::string::npos);
        return result;
    }

    std::vector<MarkerLocation> ShaderPreprocessor::scanMaterialMarkers(const std::string& source) {
        std::vector<MarkerLocation> markers;
        int lineNum = 1;
        size_t lineStart = 0;

        for (size_t i = 0; i <= source.size(); ++i) {
            if (i == source.size() || source[i] == '\n') {
                // Skenuj řádek source[lineStart..i) na `@MATERIAL_*` tokeny.
                size_t pos = lineStart;
                while (pos < i) {
                    if (source[pos] != '@') {
                        ++pos;
                        continue;
                    }
                    // Načti identifikátor za '@'.
                    size_t end = pos + 1;
                    while (end < i && (std::isalnum(static_cast<unsigned char>(source[end])) || source[end] == '_')) {
                        ++end;
                    }
                    if (end > pos + 1) {
                        std::string token(source, pos, end - pos);
                        if (token.starts_with("@MATERIAL_")) {
                            markers.push_back({std::move(token), lineNum});
                        }
                    }
                    pos = end;
                }
                ++lineNum;
                lineStart = i + 1;
            }
        }
        return markers;
    }

    void ShaderPreprocessor::validateUniqueMarkers(const std::vector<MarkerLocation>& markers,
                                                   const std::string& sourceLabel) {
        std::map<std::string, int> firstSeen;
        for (const auto& m : markers) {
            const auto [it, inserted] = firstSeen.try_emplace(m.name, m.line);
            if (!inserted) {
                std::ostringstream msg;
                msg << "[ShaderPreprocessor] " << sourceLabel << ": marker '" << m.name
                    << "' duplicated at line " << m.line << " (first seen at line " << it->second << ")";
                throw shader_placeholder_error(msg.str());
            }
        }
    }

    void ShaderPreprocessor::warnUnknownMarkers(const std::vector<MarkerLocation>& markers,
                                                const std::string& sourceLabel) {
        for (const auto& m : markers) {
            if (!Manager::isKnownMaterialPlaceholder(m.name)) {
                std::cerr << "[ShaderPreprocessor] " << sourceLabel << ":" << m.line
                          << ": warning - unknown marker '" << m.name
                          << "' (not in Manager::kKnownMaterialPlaceholders)\n";
            }
        }
    }

    void ShaderPreprocessor::validateSnippetsMatch(const std::map<std::string, std::string>& snippets,
                                                   const std::set<std::string>& presentMarkers) {
        for (const auto& [marker, _] : snippets) {
            if (!presentMarkers.contains(marker)) {
                std::ostringstream msg;
                msg << "[ShaderPreprocessor] feature provides snippet for marker '" << marker
                    << "' but no master shader source contains this placeholder";
                throw shader_placeholder_error(msg.str());
            }
        }
    }

    std::string ShaderPreprocessor::applySnippets(const std::string& source,
                                                  const std::map<std::string, std::string>& snippets) {
        std::string result = source;
        for (const auto& [marker, snippet] : snippets) {
            if (snippet.empty()) continue;
            // injectSnippet je no-op když marker není v source - stage který
            // tento marker nemá zůstane nezměněn (legitimní u multi-stage).
            result = injectSnippet(result, marker, snippet);
        }
        return result;
    }

    std::string ShaderPreprocessor::processMaterialPlaceholders(
        const std::string& source,
        const std::string& sourceLabel,
        const std::map<std::string, std::string>& snippets) {
        const auto markers = scanMaterialMarkers(source);
        validateUniqueMarkers(markers, sourceLabel);
        warnUnknownMarkers(markers, sourceLabel);

        std::set<std::string> presentSet;
        for (const auto& m : markers) {
            presentSet.insert(m.name);
        }
        validateSnippetsMatch(snippets, presentSet);

        return applySnippets(source, snippets);
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
