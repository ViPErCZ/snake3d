#ifndef SNAKE3_IMATERIALFEATURE_H
#define SNAKE3_IMATERIALFEATURE_H

#include <map>
#include <memory>
#include <string>

#include "../../../../Manager/ShaderFeature.h"
#include "../../../../Manager/ShaderManager.h"
#include "../RenderContext.h"

namespace Feature {
    // Kompozit material building block - jeden samostatný kus state +
    // bind logiky (např. HoleMap, PBR uniforms, planar reflection,
    // rain ripple). MaterialBuilder skládá features do MaterialInstance,
    // jejich flag() OR-summou určí permutaci shaderu v ShaderRegistry.
    //
    // Features jsou stateful (drží textury, lights, parameters), ale
    // přístupné read-only z `bind()`. Materiály jsou klonovatelné -
    // každá konkrétní feature implementuje `clone()` (deep instance,
    // sdílené resources přes shared_ptr).
    class IMaterialFeature {
    public:
        virtual ~IMaterialFeature() = default;

        // Vrátí bit který se OR-uje do feature mask materiálu. Definuje
        // `#define FEATURE_X` který shader uvidí.
        [[nodiscard]] virtual Manager::ShaderFeatureMask flag() const = 0;

        // Volá se z MaterialInstance::bind před každým draw call. Program
        // už je aktivní (`use()` zavolán). Feature zapíše své uniformy a
        // bindne textury.
        virtual void bind(Manager::ShaderManager& shader,
                          const Material::RenderContext& ctx) const = 0;

        // Volitelně odbinduje textury / resetuje state. Volá se po draw.
        virtual void unbind(Manager::ShaderManager& shader) const { (void)shader; }

        // Hluboká kopie instance featury - hodnoty (color, enabled, ...)
        // se klonují, sdílené resources (textury, lights) se nesdílejí
        // přes shared_ptr.
        [[nodiscard]] virtual std::shared_ptr<IMaterialFeature> clone() const = 0;

        // Volitelně: snippety, které tato feature injektuje do master shaderu.
        // Klíč je marker (např. `@MATERIAL_FRAGMENT_POST`), hodnota je cesta
        // k .glsl souboru. Cache klíč v ShaderRegistry zahrnuje cestu, takže
        // různé snippety produkují různé permutace programu. Default je prázdná
        // mapa - většina features funguje přes `#ifdef FEATURE_X` v master
        // shaderu, ne přes snippet injection.
        [[nodiscard]] virtual std::map<std::string, std::string> snippetPaths() const {
            return {};
        }
    };
} // Feature

#endif //SNAKE3_IMATERIALFEATURE_H
