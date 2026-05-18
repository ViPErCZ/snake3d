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
        auto program = registry.get({master, featureMask()});
        return std::make_shared<MaterialInstance>(std::move(program), features);
    }
} // Material
