#ifndef SNAKE3_HOLEMAPFEATURE_H
#define SNAKE3_HOLEMAPFEATURE_H

#include <snake3d/Renderer/Opengl/Material/Feature/IMaterialFeature.h>
#include <snake3d/Manager/TextureManager.h>

namespace Feature {
    // Hole map discard. Drží greyscale texturu (1 = díra → discard), binduje
    // ji na TextureSlots::HoleMap a aktivuje uniform hasHoleMap.
    //
    // C2b: dříve gateno přes `#ifdef FEATURE_HOLE_MAP` v master shaderu. Teď
    // se injektuje jako snippet do @MATERIAL_FRAGMENT_PRE - master se zbavil
    // jednoho ifdef bloku, feature je samostatně držený kód. `flag()` vrací
    // 0, protože už neexistuje odpovídající compile-time přepínač.
    class HoleMapFeature final : public IMaterialFeature {
    public:
        explicit HoleMapFeature(std::shared_ptr<Manager::TextureManager> holeMap);

        [[nodiscard]] Manager::ShaderFeatureMask flag() const override;
        void bind(Manager::ShaderProgram& shader,
                  const Material::RenderContext& ctx) const override;
        void unbind(Manager::ShaderProgram& shader) const override;
        [[nodiscard]] std::shared_ptr<IMaterialFeature> clone() const override;

        [[nodiscard]] std::map<Manager::MaterialSlot, std::string> snippetPaths() const override {
            return {{Manager::MaterialSlot::FragmentPre, "Assets/Shaders/snippets/hole_map_discard.glsl"}};
        }

        void setTexture(std::shared_ptr<Manager::TextureManager> texture) { holeMap = std::move(texture); }
        [[nodiscard]] std::shared_ptr<Manager::TextureManager> getTexture() const { return holeMap; }

    private:
        std::shared_ptr<Manager::TextureManager> holeMap;
    };
} // Feature

#endif //SNAKE3_HOLEMAPFEATURE_H
