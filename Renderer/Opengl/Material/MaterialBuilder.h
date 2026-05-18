#ifndef SNAKE3_MATERIALBUILDER_H
#define SNAKE3_MATERIALBUILDER_H

#include <memory>
#include <string>
#include <vector>

#include "Feature/IMaterialFeature.h"
#include "MaterialInstance.h"
#include "../../../Manager/ShaderFeature.h"
#include "../../../Manager/ShaderRegistry.h"

namespace Material {
    // Fluent API pro sestavení MaterialInstance z features.
    //
    // Použití:
    //   auto mat = MaterialBuilder()
    //       .useMaster("basicShader")
    //       .with(std::make_shared<HoleMapFeature>(holeTex))
    //       .build(*registry);
    //
    // build() spočítá ShaderFeatureMask jako OR všech feature->flag()
    // a získá z registru program s touto permutací. Features jsou pak
    // předány do MaterialInstance v původním pořadí.
    class MaterialBuilder {
    public:
        MaterialBuilder& useMaster(std::string name);
        MaterialBuilder& with(std::shared_ptr<Feature::IMaterialFeature> feature);

        [[nodiscard]] const std::string& masterName() const { return master; }
        [[nodiscard]] Manager::ShaderFeatureMask featureMask() const;
        [[nodiscard]] const std::vector<std::shared_ptr<Feature::IMaterialFeature>>& featuresView() const { return features; }

        [[nodiscard]] std::shared_ptr<MaterialInstance> build(Manager::ShaderRegistry& registry) const;

    private:
        std::string master;
        std::vector<std::shared_ptr<Feature::IMaterialFeature>> features;
    };
} // Material

#endif //SNAKE3_MATERIALBUILDER_H
