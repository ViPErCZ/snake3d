#ifndef SNAKE3_FEATUREREGISTRY_H
#define SNAKE3_FEATUREREGISTRY_H

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <nlohmann/json_fwd.hpp>

namespace Manager { class ResourceManager; }
namespace Feature { class IMaterialFeature; }

namespace Resource {
    // Factory signature pro static material features (albedo, normalMap, ...).
    //
    // Bere JSON entry pro daný feature (např. {"type":"albedo","color":[...]})
    // a ResourceManager pro resolve textur. Vrací konstruovanou feature, kterou
    // MaterialLoader appendne do builderu.
    //
    // Runtime-wired features (lighting/shadow/fog) NEPROCHÁZÍ tímto registry -
    // ty drží jen bool flag v MaterialSpec a callsite si vyrobí instanci s
    // live ukazately. Viz MaterialLoader.h.
    using FeatureFactory = std::function<
        std::shared_ptr<Feature::IMaterialFeature>(const nlohmann::json&, const Manager::ResourceManager&)
    >;

    // Runtime registry stringového typu -> factory. Mirror pattern Manager::ShaderRegistry.
    //
    // Bootstrap: ResourceManager ctor volá registerBuiltinFeatures() auto. Custom
    // features registruje game/plugin přes registerFeature() po construction.
    class FeatureRegistry {
    public:
        void registerFeature(const std::string& type, FeatureFactory factory);

        [[nodiscard]] bool has(const std::string& type) const;

        // Vrátí nullptr + std::cerr warning pro neznámý typ (match MaterialLoader behavior).
        // Nikdy nehází.
        [[nodiscard]] std::shared_ptr<Feature::IMaterialFeature>
            create(const std::string& type,
                   const nlohmann::json& entry,
                   const Manager::ResourceManager& rm) const;

    private:
        std::unordered_map<std::string, FeatureFactory> factories;
    };

    // Registruje 7 built-in static features: albedo, normalMap, specular, pbr,
    // uvTransform, bones, ibl. Volá se z ResourceManager ctoru.
    void registerBuiltinFeatures(FeatureRegistry& registry);
} // Resource

#endif //SNAKE3_FEATUREREGISTRY_H
