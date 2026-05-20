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
        // C2a + C3: Sesbírej snippety od všech features. Feature deklaruje
        // sloty jako typed enum (MaterialSlot), builder ho mapuje na stringový
        // marker pro ShaderHandle (ten musí být string, aby šel matchnout proti
        // textu master shaderu). Dvě features sdílející slot = appendnou se do
        // vectoru v pořadí registrace; ShaderRegistry concatenuje obsah s
        // newline mezi.
        for (const auto& f : features) {
            if (!f) continue;
            for (const auto& [slot, path] : f->snippetPaths()) {
                handle.snippets[std::string(Manager::slotMarker(slot))].push_back(path);
            }
        }
        auto program = registry.get(handle);
        return std::make_shared<MaterialInstance>(std::move(program), features);
    }
} // Material
