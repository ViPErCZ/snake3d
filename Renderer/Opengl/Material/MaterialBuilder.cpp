#include "MaterialBuilder.h"

namespace Material {
    MaterialBuilder& MaterialBuilder::useMaster(std::string name) {
        master = std::move(name);
        return *this;
    }

    MaterialBuilder& MaterialBuilder::with(std::shared_ptr<Feature::IMaterialFeature> feature) {
        if (feature) {
            features.push_back(std::move(feature));
        }
        return *this;
    }

    Manager::ShaderFeatureMask MaterialBuilder::featureMask() const {
        Manager::ShaderFeatureMask mask = 0;
        for (const auto& f : features) {
            if (f) mask |= f->flag();
        }
        return mask;
    }

    std::shared_ptr<MaterialInstance> MaterialBuilder::build(Manager::ShaderRegistry& registry) const {
        Manager::ShaderHandle handle{master, featureMask(), {}};
        // Sesbírej snippety od všech features. Pokud více features sdílí marker,
        // poslední vyhraje (deterministic per vector order).
        for (const auto& f : features) {
            if (!f) continue;
            for (const auto& [marker, path] : f->snippetPaths()) {
                handle.snippets[marker] = path;
            }
        }
        auto program = registry.get(handle);
        return std::make_shared<MaterialInstance>(std::move(program), features);
    }
} // Material
