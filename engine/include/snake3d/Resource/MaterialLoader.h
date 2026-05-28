#ifndef SNAKE3_MATERIALLOADER_H
#define SNAKE3_MATERIALLOADER_H

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include <snake3d/Renderer/Opengl/Material/MaterialBuilder.h>
#include <snake3d/Tools/Blending.h>

namespace Manager { class ResourceManager; }

namespace Resource {
    // Hodnotový kontrakt mezi MaterialLoader a callsitem.
    //
    // Static features (albedo, normalMap, specular, pbr, uvTransform, bones, ibl)
    // jsou už appendnuty do `builder` v původním JSON pořadí.
    //
    // Runtime-wired features (lighting, shadow, fog) drží jen bool flag - callsite
    // si vyrobí konkrétní instanci s live ukazately (světla, depth texture, sdílený
    // FogFeature singleton) a appendne ji sám v té samé pozici, kde stál JSON
    // záznam. Pořadí features v JSON musí matchnout C++ insertion order, protože
    // shader permutation hash závisí na něm.
    struct MaterialSpec {
        Material::MaterialBuilder builder;
        bool hasLighting{false};
        bool hasShadow{false};
        bool hasFog{false};
        Tools::Blending blending{Tools::Blending::Opaque};
    };

    [[nodiscard]] MaterialSpec loadFromFile(const std::string& path,
                                            const Manager::ResourceManager& rm);
    [[nodiscard]] MaterialSpec loadFromJson(const nlohmann::json& j,
                                            const Manager::ResourceManager& rm);
} // Resource

#endif //SNAKE3_MATERIALLOADER_H
