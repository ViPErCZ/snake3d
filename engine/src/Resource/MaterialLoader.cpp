#include <snake3d/Resource/MaterialLoader.h>

#include <fstream>
#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Renderer/Opengl/Material/Feature/AlbedoFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/BonesFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IblFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/PbrFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/SpecularFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/UvTransformFeature.h>

namespace Resource {
    namespace {
        // Vyzvedne texturu z ResourceManagera podle klíče v JSONu, nebo vrátí
        // nullptr když klíč chybí. Lazy-load pattern (BarrelNode3D) se opírá
        // o nullptr → vlastnictví textury převezme až callsite po async modelu.
        std::shared_ptr<Manager::TextureManager> textureFromJson(
            const nlohmann::json& entry,
            const Manager::ResourceManager& rm) {
            if (!entry.contains("texture")) return nullptr;
            return rm.getTexture(entry.at("texture").get<std::string>());
        }

        glm::vec2 vec2FromJson(const nlohmann::json& arr, const glm::vec2 fallback) {
            if (!arr.is_array() || arr.size() < 2) return fallback;
            return {arr.at(0).get<float>(), arr.at(1).get<float>()};
        }

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

        void handleAlbedo(const nlohmann::json& entry,
                          const Manager::ResourceManager& rm,
                          MaterialSpec& spec) {
            auto f = std::make_shared<Feature::AlbedoFeature>(textureFromJson(entry, rm));
            if (entry.contains("color")) {
                const auto& c = entry.at("color");
                if (c.is_array() && c.size() >= 3) {
                    f->setColor(glm::vec3(c.at(0).get<float>(),
                                          c.at(1).get<float>(),
                                          c.at(2).get<float>()));
                }
            }
            if (entry.contains("alpha")) {
                f->setAlpha(entry.at("alpha").get<float>());
            }
            if (entry.contains("ambientIntensity")) {
                f->setAmbientIntensity(entry.at("ambientIntensity").get<float>());
            }
            spec.builder.with(f);
        }

        void handleNormalMap(const nlohmann::json& entry,
                             const Manager::ResourceManager& rm,
                             MaterialSpec& spec) {
            auto f = std::make_shared<Feature::NormalMapFeature>(textureFromJson(entry, rm));
            spec.builder.with(f);
        }

        void handleSpecular(const nlohmann::json& entry,
                            const Manager::ResourceManager& rm,
                            MaterialSpec& spec) {
            const float shininess = entry.contains("shininess")
                ? entry.at("shininess").get<float>()
                : 32.0f;
            auto f = std::make_shared<Feature::SpecularFeature>(textureFromJson(entry, rm), shininess);
            spec.builder.with(f);
        }

        void handlePbr(const nlohmann::json& entry,
                       const Manager::ResourceManager& rm,
                       MaterialSpec& spec) {
            // PBR feature potřebuje metalness + roughness povinně, aoMap volitelně.
            // Klíče v JSONu nesou jména textur, ne hodnoty - resolve přes ResourceManager.
            const auto getTex = [&](const char* key) -> std::shared_ptr<Manager::TextureManager> {
                if (!entry.contains(key)) return nullptr;
                return rm.getTexture(entry.at(key).get<std::string>());
            };
            auto f = std::make_shared<Feature::PbrFeature>(
                getTex("metalness"),
                getTex("roughness"),
                getTex("aoMap"));
            spec.builder.with(f);
        }

        void handleUvTransform(const nlohmann::json& entry,
                               MaterialSpec& spec) {
            glm::vec2 scale(1.0f);
            glm::vec2 offset(0.0f);
            if (entry.contains("scale"))  scale  = vec2FromJson(entry.at("scale"),  scale);
            if (entry.contains("offset")) offset = vec2FromJson(entry.at("offset"), offset);
            auto f = std::make_shared<Feature::UvTransformFeature>(scale, offset);
            spec.builder.with(f);
        }

        void handleBones(const nlohmann::json& entry,
                         MaterialSpec& spec) {
            const bool useBones = entry.contains("useBones")
                ? entry.at("useBones").get<bool>()
                : false;
            spec.builder.with(std::make_shared<Feature::BonesFeature>(useBones));
        }

        void handleIbl(const nlohmann::json& entry,
                       const Manager::ResourceManager& rm,
                       MaterialSpec& spec) {
            spec.builder.with(std::make_shared<Feature::IblFeature>(textureFromJson(entry, rm)));
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

            if (type == "albedo") {
                handleAlbedo(entry, rm, spec);
            } else if (type == "normalMap") {
                handleNormalMap(entry, rm, spec);
            } else if (type == "specular") {
                handleSpecular(entry, rm, spec);
            } else if (type == "pbr") {
                handlePbr(entry, rm, spec);
            } else if (type == "uvTransform") {
                handleUvTransform(entry, spec);
            } else if (type == "bones") {
                handleBones(entry, spec);
            } else if (type == "ibl") {
                handleIbl(entry, rm, spec);
            } else if (type == "lighting") {
                spec.hasLighting = true;
            } else if (type == "shadow") {
                spec.hasShadow = true;
            } else if (type == "fog") {
                spec.hasFog = true;
            } else {
                std::cerr << "[MaterialLoader] unknown feature type: " << type << "\n";
            }
        }

        return spec;
    }
} // Resource
