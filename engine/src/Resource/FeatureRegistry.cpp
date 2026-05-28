#include <snake3d/Resource/FeatureRegistry.h>

#include <iostream>
#include <utility>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

#include <snake3d/Manager/ResourceManager.h>
#include <snake3d/Renderer/Opengl/Material/Feature/AlbedoFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/BonesFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IblFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/NormalMapFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/PbrFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/SpecularFeature.h>
#include <snake3d/Renderer/Opengl/Material/Feature/UvTransformFeature.h>

namespace Resource {
    namespace {
        // Vyzvedne texturu z ResourceManagera podle klíče v JSONu, nebo vrátí
        // nullptr když klíč chybí. Lazy-load pattern (BarrelNode3D) se opírá
        // o nullptr - vlastnictví textury převezme až callsite po async modelu.
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
    } // anonymous

    void FeatureRegistry::registerFeature(const std::string& type, FeatureFactory factory) {
        factories[type] = std::move(factory);
    }

    bool FeatureRegistry::has(const std::string& type) const {
        return factories.find(type) != factories.end();
    }

    std::shared_ptr<Feature::IMaterialFeature> FeatureRegistry::create(
        const std::string& type,
        const nlohmann::json& entry,
        const Manager::ResourceManager& rm) const {
        const auto it = factories.find(type);
        if (it == factories.end()) {
            std::cerr << "[FeatureRegistry] unknown feature type: " << type << "\n";
            return nullptr;
        }
        return it->second(entry, rm);
    }

    void registerBuiltinFeatures(FeatureRegistry& registry) {
        registry.registerFeature("albedo",
            [](const nlohmann::json& entry, const Manager::ResourceManager& rm)
                -> std::shared_ptr<Feature::IMaterialFeature> {
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
            return f;
        });

        registry.registerFeature("normalMap",
            [](const nlohmann::json& entry, const Manager::ResourceManager& rm)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            return std::make_shared<Feature::NormalMapFeature>(textureFromJson(entry, rm));
        });

        registry.registerFeature("specular",
            [](const nlohmann::json& entry, const Manager::ResourceManager& rm)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            const float shininess = entry.contains("shininess")
                ? entry.at("shininess").get<float>()
                : 32.0f;
            return std::make_shared<Feature::SpecularFeature>(textureFromJson(entry, rm), shininess);
        });

        registry.registerFeature("pbr",
            [](const nlohmann::json& entry, const Manager::ResourceManager& rm)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            // PBR feature potřebuje metalness + roughness povinně, aoMap volitelně.
            // Klíče v JSONu nesou jména textur, ne hodnoty - resolve přes ResourceManager.
            const auto getTex = [&](const char* key) -> std::shared_ptr<Manager::TextureManager> {
                if (!entry.contains(key)) return nullptr;
                return rm.getTexture(entry.at(key).get<std::string>());
            };
            return std::make_shared<Feature::PbrFeature>(
                getTex("metalness"),
                getTex("roughness"),
                getTex("aoMap"));
        });

        registry.registerFeature("uvTransform",
            [](const nlohmann::json& entry, const Manager::ResourceManager&)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            glm::vec2 scale(1.0f);
            glm::vec2 offset(0.0f);
            if (entry.contains("scale"))  scale  = vec2FromJson(entry.at("scale"),  scale);
            if (entry.contains("offset")) offset = vec2FromJson(entry.at("offset"), offset);
            return std::make_shared<Feature::UvTransformFeature>(scale, offset);
        });

        registry.registerFeature("bones",
            [](const nlohmann::json& entry, const Manager::ResourceManager&)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            const bool useBones = entry.contains("useBones")
                ? entry.at("useBones").get<bool>()
                : false;
            return std::make_shared<Feature::BonesFeature>(useBones);
        });

        registry.registerFeature("ibl",
            [](const nlohmann::json& entry, const Manager::ResourceManager& rm)
                -> std::shared_ptr<Feature::IMaterialFeature> {
            return std::make_shared<Feature::IblFeature>(textureFromJson(entry, rm));
        });
    }
} // Resource
