#include <snake3d/Resource/MaterialLoader.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Resource/FeatureRegistry.h>

namespace Resource {
    namespace {
        Tools::Blending parseBlending(const std::string& s) {
            if (s == "opaque")        return Tools::Blending::Opaque;
            if (s == "translucent")   return Tools::Blending::Translucent;
            if (s == "additive")      return Tools::Blending::Additive;
            if (s == "alphaAdditive") return Tools::Blending::AlphaAdditive;
            if (s == "modulate")      return Tools::Blending::Modulate;
            if (s == "text")          return Tools::Blending::Text;
            std::cerr << "[MaterialLoader] unknown blending mode: " << s
                      << " (falling back to opaque)\n";
            return Tools::Blending::Opaque;
        }
    } // anonymous

    MaterialSpec loadFromFile(const std::string& path,
                              const Manager::ResourceManager& rm) {
        std::ifstream f(path);
        if (!f.is_open()) {
            throw std::runtime_error("[MaterialLoader] cannot open material file: " + path);
        }
        nlohmann::json j;
        f >> j;
        return loadFromJson(j, rm);
    }

    MaterialSpec loadFromJson(const nlohmann::json& j,
                              const Manager::ResourceManager& rm) {
        // H5: routuje přes FeatureRegistry z ResourceManager (auto-bootstrap
        // built-in features v ResourceManager ctoru). Pokud volající registry
        // nenastavil (zřídka kdy), padá to do registry=nullptr větve, která
        // jen warninguje unknown typ - tj. žádné features se neaplikujou.
        return loadFromJson(j, rm, rm.getFeatureRegistry().get());
    }

    MaterialSpec loadFromJson(const nlohmann::json& j,
                              const Manager::ResourceManager& rm,
                              const FeatureRegistry* registry) {
        MaterialSpec spec;

        if (j.contains("master")) {
            spec.builder.useMaster(j.at("master").get<std::string>());
        }
        if (j.contains("blending")) {
            spec.blending = parseBlending(j.at("blending").get<std::string>());
        }

        if (!j.contains("features") || !j.at("features").is_array()) {
            return spec;
        }

        for (const auto& entry : j.at("features")) {
            if (!entry.contains("type")) {
                std::cerr << "[MaterialLoader] feature entry missing 'type', skipping\n";
                continue;
            }
            const auto type = entry.at("type").get<std::string>();

            // Runtime-wired features (live C++ pointers, JSON nese jen flag).
            if (type == "lighting") { spec.hasLighting = true; continue; }
            if (type == "shadow")   { spec.hasShadow = true; continue; }
            if (type == "fog")      { spec.hasFog = true; continue; }

            // Static features přes registry dispatch.
            if (registry && registry->has(type)) {
                if (auto feature = registry->create(type, entry, rm)) {
                    spec.builder.with(feature);
                }
                continue;
            }

            std::cerr << "[MaterialLoader] unknown feature type: " << type << "\n";
        }

        return spec;
    }
} // Resource
